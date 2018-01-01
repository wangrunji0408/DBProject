//
// Created by 王润基 on 2017/12/13.
//

#include <bitset>
#include <ast/Exceptions.h>
#include <set>
#include "Table.h"
#include "system/Database.h"
#include "TableRecordRef.h"

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

	for(auto const& record: records) {
		auto error = check(record);
		if(!error.empty())
			throw ExecuteError(error);
	}

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
					auto tr = TableRecordRef(meta, record.data);
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

void Table::insert(std::vector<TableRecord> const &records) {
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

	checkInsertValues(records);

	for(auto const& record: records) {
		auto rid = recordSet->insert(toData(record).data());
		for(int i=0; i<indexColCount; ++i)
			if(!record.isNullAtCol(colIDs[i]))
				indexs[i]->insertEntry(record.getDataAtCol(colIDs[i]).data(), rid);
	}
	meta->recordCount += records.size();
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

SelectResult Table::select(Condition const &condition) {
	auto result = SelectResult();
	auto predict = makePredict(condition);
	for(auto iter = recordSet->iterateRecords(); iter.hasNext(); ) {
		auto record = iter.getNext();
		if(predict(record.data))
			result.records.push_back(toRecord(record.data));
	}
	return result;
}

std::string Table::check(TableRecord const &record) const {
	if(record.size() != meta->columnSize)
		return "Value attr size not equal to column size";
	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		auto t1 = record.getTypeAtCol(i);
		auto t2 = col.dataType;
		if(t1 != t2)
			return "Column " + std::to_string(i) + " type error";
		if(record.getDataAtCol(i).size() > col.size)
			return "Column " + std::to_string(i) + " size exceed";
	}
	return "";
}

Data Table::toData(const TableRecord &value) const {
	auto data = Data(meta->recordLength);
	auto nullBitsetPtr = (std::bitset<128>*)(static_cast<const void*>(
			data.data() + meta->recordLength - (meta->columnSize + 7) / 8));
	for(int i=0; i<meta->columnSize; ++i) {
		auto& col = meta->columns[i];
		auto isNull = value.isNullAtCol(i);
		nullBitsetPtr->set(static_cast<size_t>(i), isNull);
		if(!isNull) {
			auto const& v = value.getDataAtCol(i);
			std::memcpy(data.data() + col.offset, v.data(), v.size());
		}
	}
	return data;
}

TableRecord Table::toRecord(const uchar *data) const {
	auto record = TableRecord();
	auto value = TableRecordRef(meta, data);
	for(int i=0; i<meta->columnSize; ++i) {
		auto type = meta->columns[i].dataType;
		if(value.isNullAtCol(i))
			record.pushNull(type);
		else
			record.push(type, value.getDataAtCol(i));
	}
	return record;
}
