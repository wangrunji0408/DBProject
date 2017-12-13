//
// Created by 王润基 on 2017/12/13.
//

#include <ast/Exceptions.h>
#include "Table.h"
#include "Database.h"

Table::Table(RecordSet* rs, Database const& database) :
		id(rs->id), metaPageID(rs->tablePageID), name(rs->name),
		recordSet(rs), database(database) {}

TableDef Table::getDef() const {
	auto metaPage = database.getPage(metaPageID);
	auto meta = (TableMetaPage*)metaPage.getDataReadonly();
	return meta->toDef(database);
}

void Table::checkInsertValues(std::vector<CommandDef::RecordValue> const &values) const {
	auto errors = std::vector<ValueError>();

	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		if(!col.nullable) {
			// Check null value
			int j = 0;
			for(auto const& v: values) {
				if(v.values[i].empty())
					errors.push_back(NotNullableError(j, v, i, col.name));
				++j;
			}
		}
		// TODO check unique
//		if(col.unique) {
//			if(col.indexID != -1) {
//				auto index = database.getIndexManager()->getIndex(col.indexID);
//
//			} else {
//
//			}
//		}
	}

	if(!errors.empty())
		throw errors;
}

void Table::insert(std::vector<CommandDef::RecordValue> const &values) {
	static uchar recordBuf[TableMetaPage::MAX_RECORD_LENGTH];
	static std::unique_ptr<Index> indexs[TableMetaPage::MAX_COLUMN_SIZE];
	static short offsets[TableMetaPage::MAX_COLUMN_SIZE];

	auto metaPage = database.getPage(metaPageID);
	meta = (TableMetaPage*)metaPage.getDataReadonly();

	// statistic index
	int indexColCount = 0;
	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		if(col.indexID != -1) {
			indexs[indexColCount] = database.getIndexManager()->getIndex(col.indexID);
			offsets[indexColCount] = col.offset;
			indexColCount++;
		}
	}

	checkInsertValues(values);

	for(auto const& value: values) {
		try {
			makeRecordData(recordBuf, value);
		} catch (std::exception const& e) {
			throw ParseError(string("Data format error: ") + e.what());
		}
		auto rid = recordSet->insertRecord(recordBuf);
		for(int i=0; i<indexColCount; ++i)
			indexs[i]->insertEntry(recordBuf + offsets[i], rid);
	}
}

void Table::makeRecordData(uchar *buf, CommandDef::RecordValue const &value) const {
	if(value.values.size() != meta->columnSize)
		throw ExecuteError("Value attr size not equal to column size");
	auto nullBitsetPtr = static_cast<bitset<128>*>(static_cast<void*>(
			buf + meta->recordLength - (meta->columnSize + 7) / 8));
	for(int i=0; i<meta->columnSize; ++i) {
		auto& col = meta->columns[i];
		nullBitsetPtr->set(static_cast<size_t>(i), value.values[i].empty());
		parse(value.values[i], buf + col.offset, col.dataType, col.size);
	}
}
