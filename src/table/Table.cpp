//
// Created by 王润基 on 2017/12/13.
//

#include <ast/Exceptions.h>
#include <set>
#include "Table.h"
#include "system/Database.h"

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

void Table::checkInsertValues(std::vector<TableRecord> const &records) const {

	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		if(!col.nullable || col.primaryKey) { // TODO handle multiple primary key
			// Check null value
			for(auto const& record: records)
				if(record.isNullAtCol(i))
					throw ExecuteError("Value can not be null");
		}
		if(col.unique || col.primaryKey) { // TODO handle multiple primary key
			if(col.indexID != -1) {
				auto index = database.getIndexManager()->getIndex(col.indexID);
				auto vs = std::set<Data>();
				for(auto const& record: records) {
					auto data = record.getDataAtCol(i);
					if(index->containsEntry(data.data()) || vs.find(data) != vs.end())
						throw ExecuteError("Value is not unique");
					vs.insert(std::move(data));
				}
			} else {
				auto vs = std::set<Data>();
				// Insert all col value in recordSet into vs
				for(auto iter = recordSet->iterateRecords(); iter.hasNext(); ) {
					auto record = iter.getNext();
					auto tr = TableRecord(meta, record.data);
					if(!tr.isNullAtCol(i))
						vs.insert(tr.getDataAtCol(i));
				}
				for(auto const& record: records) {
					if(!record.isNullAtCol(i)) {
						auto data = record.getDataAtCol(i);
						if(vs.find(data) != vs.end())
							throw ExecuteError("Value is not unique");
						vs.insert(std::move(data));
					}
				}
			}
		}
	}
}

void Table::insert(std::vector<RecordValue> const &values) {
	static std::unique_ptr<Index> indexs[TableMetaPage::MAX_COLUMN_SIZE];
	static short colIDs[TableMetaPage::MAX_COLUMN_SIZE];

	// statistic index
	int indexColCount = 0;
	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		if(col.indexID != -1) {
			indexs[indexColCount] = database.getIndexManager()->getIndex(col.indexID);
			colIDs[indexColCount] = static_cast<short>(i);
			indexColCount++;
		}
	}

	auto records = std::vector<TableRecord>();
	records.reserve(values.size());
	for(auto const& v: values) {
		try {
			records.emplace_back(meta, v);
		} catch (std::exception const& e) {
			throw ExecuteError("Failed to convert RecordValue to TableRecord");
		}
	}

	checkInsertValues(records);

	for(auto const& value: values) {
		auto record = TableRecord(meta, value);
		auto rid = recordSet->insert(record.getDataRef());
		for(int i=0; i<indexColCount; ++i)
			if(!record.isNullAtCol(colIDs[i]))
				indexs[i]->insertEntry(record.getDataRefAtCol(colIDs[i]), rid);
	}
	meta->recordCount += values.size();
	metaPage.getDataMutable();
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
	for(auto const& record: newRecords) {
		recordSet->update(record);
		delete[] record.data;
	}
}

int Table::size() const {
	return meta->recordCount;
}
