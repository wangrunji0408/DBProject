//
// Created by 王润基 on 2017/11/7.
//

#include "TableMetaPage.h"
#include "system/Database.h"

void TableMetaPage::makeFromDef(TableDef const &def, Database const& database) {
	std::memset(this, 0, sizeof(TableMetaPage));
	firstPageID = -1;
	recordLength = 0;
	recordCount = 0;

	if(def.name.length() > MAX_NAME_LENGTH)
		throw std::runtime_error("The length of table name is too large");
	std::strncpy(name, def.name.c_str(), MAX_NAME_LENGTH);

	columnSize = static_cast<short>(def.columns.size());
	for(int i=0; i<columnSize; ++i) {
		auto const& col = def.columns[i];
		columns[i].makeFromDef(col);
		columns[i].offset = static_cast<short>(recordLength);
		recordLength += columns[i].size;	// if (VAR)CHAR then size++
	}
	int nullBitsetBytes = (columnSize + 7) / 8;
	recordLength += nullBitsetBytes;
	if(recordLength > MAX_RECORD_LENGTH)
		throw std::runtime_error("The length of record is too large");

	for(auto const& fk: def.foreignKeys) {
		int colId = getColomnId(fk.keyName);
		if(colId == -1)
			throw std::runtime_error("Foreign key name not exist");
		RecordSet* table;
		try {
			table = database.recordManager->getSet(fk.refTable);
		} catch (std::exception const& e) {
			throw std::runtime_error("Foreign key ref table name not exist");
		}
		auto metaPage = database.getPage(table->tablePageID);
		auto meta = (TableMetaPage*)metaPage.getDataReadonly();
		auto fcolId = meta->getColomnId(fk.refName);
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

TableDef TableMetaPage::toDef(Database const& database) const {
	auto def = TableDef();
	def.name = string(name);
	for(int i=0; i<columnSize; ++i) {
		auto const& col = columns[i];
		def.columns.push_back(col.toDef());
		if(col.primaryKey)
			def.primaryKeys.emplace_back(col.name);
		if(col.foreignTableID != -1) {
			auto table = database.recordManager->getSet(col.foreignTableID);
			auto meta = (TableMetaPage*)database
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

void TableMetaPage::Column::makeFromDef(ColumnDef const &def) {
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
	if(dataType == CHAR || dataType == VARCHAR)
		size++;
}

ColumnDef TableMetaPage::Column::toDef() const {
	auto def = ColumnDef();
	def.name = name;
	def.size = static_cast<size_t>(size);
	def.dataType = dataType;
	def.unique = unique;
	def.nullable = nullable;
	if(dataType == CHAR || dataType == VARCHAR)
		def.size--;
	return def;
}
