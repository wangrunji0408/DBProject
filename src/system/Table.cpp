//
// Created by 王润基 on 2017/12/13.
//

#include <ast/Exceptions.h>
#include "Table.h"
#include "Database.h"

Table::Table(RecordSet* rs, Database const& database) :
		id(rs->id), metaPageID(rs->tablePageID), name(rs->name),
		recordSet(rs), database(database),
		metaPage(database.getPage(metaPageID))
{
	meta = (TableMetaPage*)metaPage.getDataReadonly();
}

TableDef Table::getDef() const {
	return meta->toDef(database);
}

void Table::checkInsertValues(std::vector<RecordValue> const &values) const {
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

void Table::insert(std::vector<RecordValue> const &values) {
	static uchar recordBuf[TableMetaPage::MAX_RECORD_LENGTH];
	static std::unique_ptr<Index> indexs[TableMetaPage::MAX_COLUMN_SIZE];
	static short offsets[TableMetaPage::MAX_COLUMN_SIZE];

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
		auto rid = recordSet->insert(recordBuf);
		for(int i=0; i<indexColCount; ++i)
			indexs[i]->insertEntry(recordBuf + offsets[i], rid);
	}
	meta->recordCount += values.size();
	metaPage.getDataMutable();
}

void Table::makeRecordData(uchar *buf, RecordValue const &value) const {
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

void Table::delete_(Condition const &condition) {
	auto predict = makePredict(condition);
	auto rids = std::vector<RID>();
	for(auto iter = recordSet->iterateRecords(); iter.hasNext(); ) {
		auto record = iter.getNext();
		if(predict(record.data))
			rids.push_back(record.recordID);
	}
	for(auto const& rid: rids)
		recordSet->remove(rid);
	meta->recordCount -= rids.size();
	metaPage.getDataMutable();
}

void Table::update(std::vector<SetStmt> const &sets, Condition const &condition) {
	auto predict = makePredict(condition);
	auto updateFunc = makeUpdate(sets);
	auto newRecords = std::vector<Record>();
	for(auto iter = recordSet->iterateRecords(); iter.hasNext(); ) {
		auto record = iter.getNext();
		if(predict(record.data)) {
			auto data = new char[meta->recordLength];
			std::memcpy(data, record.data, meta->recordLength);
			updateFunc(data);
			newRecords.push_back(record.copyWithNewData(data));
		}
	}
	for(auto const& record: newRecords)
		recordSet->update(record);
}

int Table::size() const {
	return meta->recordCount;
}
