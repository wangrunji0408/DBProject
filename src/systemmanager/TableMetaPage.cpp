//
// Created by 王润基 on 2017/11/7.
//

#include "TableMetaPage.h"
#include "../recordmanager/RecordManager.h"
#include "../systemmanager/Database.h"

void TableMetaPage::makeFromDef(TableDef const &def, RecordManager& recordManager) {
	std::memset(this, 0, sizeof(TableMetaPage));
	firstPageID = -1;
	recordLength = 0;

	if(def.name.length() > MAX_NAME_LENGTH)
		throw std::runtime_error("The length of table name is too large");
	std::strncpy(name, def.name.c_str(), MAX_NAME_LENGTH);

	columnSize = static_cast<short>(def.colomns.size());
	for(int i=0; i<columnSize; ++i) {
		auto const& col = def.colomns[i];
		columns[i].makeFromDef(col);
		columns[i].offset = static_cast<short>(recordLength);
		recordLength += col.size;
	}
	int nullBitsetBytes = (columnSize + 7) / 8;
	recordLength += nullBitsetBytes;
	if(recordLength > MAX_RECORD_LENGTH)
		throw std::runtime_error("The length of record is too large");

	for(auto const& fk: def.foreignKeys) {
		int colId = getColomnId(fk.keyName);
		if(colId == -1)
			throw std::runtime_error("Foreign key name not exist");
		Table* table;
		try {
			table = recordManager.getTable(fk.refTable);
		} catch (std::exception const& e) {
			throw std::runtime_error("Foreign key ref table name not exist");
		}
		auto metaPage = (TableMetaPage*)recordManager.database
				.getPage(table->tablePageID).getDataReadonly();
		auto fcolId = metaPage->getColomnId(fk.refName);
		if(fcolId == -1)
			throw std::runtime_error("Foreign key ref column name not exist");
		columns[colId].foreignTableID = (short)table->id;
		columns[colId].foreignColumnID = (short)fcolId;
	}

	for(auto const& pkName: def.primaryKeys) {
		int colId = getColomnId(pkName);
		if(colId == -1)
			throw std::runtime_error("Primary key name not exist");
		columns[colId].primaryKey = true;
	}
}

TableDef TableMetaPage::toDef(RecordManager& recordManager) const {
	auto def = TableDef();
	def.name = string(name);
	for(int i=0; i<columnSize; ++i) {
		auto const& col = columns[i];
		def.colomns.push_back(col.toDef());
		if(col.primaryKey)
			def.primaryKeys.emplace_back(col.name);
		if(col.foreignTableID != -1) {
			auto table = recordManager.getTable(col.foreignTableID);
			auto meta = (TableMetaPage*)recordManager.database
					.getPage(table->tablePageID).getDataReadonly();
			auto fk = ForeignKeyDef();
			fk.keyName = string(col.name);
			fk.refTable = table->name;
			fk.refName = meta->columns[col.foreignColumnID].name;
			def.foreignKeys.push_back(fk);
		}
	}
	return def;
}

int TableMetaPage::getColomnId(std::string name) const {
	for(int i=0; i<columnSize; ++i)
		if(name == columns[i].name)
			return i;
	return -1;
}

void TableMetaPage::Column::makeFromDef(ColomnDef const &def) {
	if(def.name.size() >= MAX_COLUMN_NAME_LENGTH)
		throw std::runtime_error("The length of column name is too large");
	std::strncpy(name, def.name.c_str(), MAX_COLUMN_NAME_LENGTH);

	size = static_cast<short>(def.size);
	dataType = def.dataType;
	nullable = def.nullable;
	unique = def.unique;
	indexID = -1;
	foreignColumnID = -1;
	foreignTableID = -1;
}

ColomnDef TableMetaPage::Column::toDef() const {
	auto def = ColomnDef();
	def.name = name;
	def.size = static_cast<size_t>(size);
	def.dataType = dataType;
	def.unique = unique;
	def.nullable = nullable;
	return def;
}
