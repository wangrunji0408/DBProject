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

inline int calcPKHash(TableRecordRef const& record, std::vector<int> pkIds) {
	int hash = 0;
	for(auto id: pkIds) {
		for(auto c: record.getDataAtCol(id))
			hash = (hash << 5) + hash + c;
	}
	return hash;
}
inline int calcPKHash(TableRecord const& record, std::vector<int> pkIds) {
	int hash = 0;
	for(auto id: pkIds) {
		for(auto c: record.getDataAtCol(id))
			hash = (hash << 5) + hash + c;
	}
	return hash;
}

void Table::checkInsertValues(std::vector<TableRecord> const &records) const {

	for(auto const& record: records) {
		auto error = check(record);
		if(!error.empty())
			throw ExecuteError(error);
	}

	auto pkIds = std::vector<int>();
	for(int i=0; i<meta->columnSize; ++i) {
		if(meta->columns[i].primaryKey)
			pkIds.push_back(i);
	}
	bool onePK = pkIds.size() == 1;

	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		if(!col.nullable || col.primaryKey) {
			// Check null value
			for(auto const& record: records)
				if(record.isNullAtCol(i))
					throw NullValueError();
		}
		if(col.unique || (col.primaryKey && onePK)) {
			if(col.indexID != -1) {
				auto index = database.getIndexManager()->getIndex(col.indexID);
				auto vs = std::set<Data>();
				for(auto const& record: records) {
					auto data = record.getDataAtCol(i);
					if(index->containsEntry(data.data()) || vs.find(data) != vs.end())
						throw NotUniqueError();
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
							throw NotUniqueError();
						vs.insert(std::move(data));
					}
				}
			}
		}
		if(col.foreignTableID != -1) {
			// check foreign key
			auto foreignTable = database.getTable(col.foreignTableID);
			auto indexID = foreignTable->meta->columns[col.foreignColumnID].indexID;
			if(indexID == -1)
				throw std::runtime_error("Foreign key without an index is not supported");
			auto index = database.getIndexManager()->getIndex(indexID);
			for(auto const& record: records) {
				auto const& data = record.getDataAtCol(i);
				if(!index->containsEntry(data.data()))
					throw ForeignKeyNotExistError();
			}
		}
	}

	// check unique for multiple primary key
	if(!onePK) {
		auto vs = std::set<int>();
		// build set
		for(auto iter = recordSet->iterateRecords(); iter.hasNext(); ) {
			auto record = iter.getNext();
			auto tr = TableRecordRef(meta, record.data);
			int hash = calcPKHash(tr, pkIds);
			vs.insert(hash);
		}
		//
		for(auto const& record: records) {
			int hash = calcPKHash(record, pkIds);
			if(vs.find(hash) != vs.end())
				throw NotUniqueError();
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

	auto rids = std::vector<RID>();
	filterThenForeach(condition, [&](Record const& record) {
		rids.push_back(record.recordID);
	});

	for(auto const& rid: rids) {
		auto record = TableRecordRef(meta, recordSet->getRecord(rid).data);
		for(int i=0; i<indexColCount; ++i)
			if(!record.isNullAtCol(colIDs[i]))
				indexs[i]->deleteEntry(record.getDataAtCol(colIDs[i]).data(), rid);
	}
	for(auto const& rid: rids)
		recordSet->remove(rid);
	meta->recordCount -= rids.size();
	metaPage.getDataMutable();
}

void Table::update(std::vector<SetStmt> const &sets, Condition const &condition) {
	auto updateFunc = makeUpdate(sets);
	auto newRecords = std::vector<Record>();
	filterThenForeach(condition, [&](Record const& record) {
		auto data = new char[meta->recordLength];
		std::memcpy(data, record.data, meta->recordLength);
		updateFunc(data);
		newRecords.push_back(record.copyWithNewData(data));
	});
	for(auto const& record: newRecords) {
		recordSet->update(record);
		delete[] record.data;
	}
}

int Table::size() const {
	return meta->recordCount;
}

SelectResult Table::select(std::vector<std::string> const& selects, Condition const &condition) {
	auto result = SelectResult();
	auto ids = std::vector<int>();
	if(selects.size() == 1 && selects[0] == "*") {
		for(int i=0; i<meta->columnSize; ++i) {
			ids.push_back(i);
			result.colNames.push_back(string(meta->name) + "." + meta->columns[i].name);
		}
	} else {
		for(const auto &name: selects) {
			auto id = meta->getColomnId(name);
			if(id == -1)
				throw NameNotExistError("column", name);
			ids.push_back(id);
			result.colNames.push_back(string(meta->name) + "." + name);
		}
	}

	filterThenForeach(condition, [&](Record const& record) {
		result.records.push_back(toRecord(record.data, ids));
	});
	return result;
}

std::string Table::check(TableRecord const &record) const {
	if(record.size() != meta->columnSize)
		return "Value attr size not equal to column size";
	for(int i=0; i<meta->columnSize; ++i) {
		auto const& col = meta->columns[i];
		auto const& value = record.getDataAtCol(i);
		auto isNull = record.isNullAtCol(i);
		auto t1 = record.getTypeAtCol(i);
		auto t2 = col.dataType;
		if((t1 == VARCHAR || t1 == CHAR) && (t2 == VARCHAR || t2 == CHAR)) {
		} else if((t1 == VARCHAR || t1 == CHAR) && t2 == DATE) {
			try {
				parseDate((char*)value.data());
			} catch (std::runtime_error const&) {
				return "Date format error";
			}
			continue;
		} else if(t1 == INT || t2 == FLOAT) {
		} else if(isNull) {
		} else if(t1 != t2) {
			return "Column " + std::to_string(i) + " type error";
		}
		if(value.size() > col.size)
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
			auto type = value.getTypeAtCol(i);
			auto const& v = value.getDataAtCol(i);
			if(col.dataType == DATE && (type == VARCHAR || type == CHAR)) {
				int x = parseDate((char*)v.data());
				std::memcpy(data.data() + col.offset, &x, sizeof(x));
			} else if(col.dataType == FLOAT && type == INT) {
				float x = *(int*)v.data();
				std::memcpy(data.data() + col.offset, &x, sizeof(x));
			} else
				std::memcpy(data.data() + col.offset, v.data(), v.size());
		}
	}
	return data;
}

TableRecord Table::toRecord(const uchar *data, std::vector<int> const& ids) const {
	auto record = TableRecord();
	auto value = TableRecordRef(meta, data);
	for(auto i: ids) {
		auto type = meta->columns[i].dataType;
		if(value.isNullAtCol(i))
			record.pushNull(type);
		else
			record.push(type, value.getDataAtCol(i));
	}
	return record;
}

void Table::filterThenForeach(Condition const &condition, std::function<void(Record const&)> const& process) {
	auto sub = selectWithIndex(condition);
	auto predict = makePredict(condition);
	if(sub.first) {
		for(auto const& rid: sub.second) {
			auto record = recordSet->getRecord(rid);
			if(predict(record.data))
				process(record);
		}
	} else {
		for(auto iter = recordSet->iterateRecords(); iter.hasNext(); ) {
			auto record = iter.getNext();
			if(predict(record.data))
				process(record);
		}
	}
}
