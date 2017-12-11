//
// Created by 王润基 on 2017/12/11.
//

#include <systemmanager/TableMetaPage.h>
#include <ast/Exceptions.h>
#include "QueryManager.h"
using namespace CommandDef;

QueryManager::QueryManager(Database &database) : database(database) {}

void QueryManager::execute(Command const &cmd) {
	switch(cmd.getType()) {
		case CMD_INSERT:
			insert((Insert const&)cmd); break;
		case CMD_UPDATE:
			update((Update const&)cmd); break;
		case CMD_DELETE:
			delete_((Delete const&)cmd); break;
		case CMD_SELECT:
			select((Select const&)cmd); break;
	}
}

CommandDef::SelectResult QueryManager::select(Select const &cmd) {
	throw std::runtime_error("Not implemented!");
}

void QueryManager::update(Update const &cmd) {
	throw std::runtime_error("Not implemented!");
}

void QueryManager::delete_(Delete const &cmd) {
	throw std::runtime_error("Not implemented!");
}

void QueryManager::insert(Insert const &cmd) {
	static uchar recordBuf[TableMetaPage::MAX_RECORD_LENGTH];
	static std::unique_ptr<Index> indexs[TableMetaPage::MAX_COLUMN_SIZE];
	static short offsets[TableMetaPage::MAX_COLUMN_SIZE];

	auto table = database.getTable(cmd.tableName);
	auto page = database.getPage(table->tablePageID);
	auto meta = *(TableMetaPage*)page.getDataReadonly();

	// statistic index
	int indexColCount = 0;
	for(int i=0; i<meta.columnSize; ++i) {
		auto const& col = meta.columns[i];
		if(col.indexID != -1) {
			indexs[indexColCount] = database.getIndexManager()->getIndex(col.indexID);
			offsets[indexColCount] = col.offset;
			indexColCount++;
		}
	}


	for(auto const& value: cmd.records) {
		try {
			makeRecordData(recordBuf, meta, value);
		} catch (std::exception const& e) {
			throw ParseError(string("Data format error: ") + e.what());
		}
		auto rid = table->insertRecord(recordBuf);
		for(int i=0; i<indexColCount; ++i)
			indexs[i]->insertEntry(recordBuf + offsets[i], rid);
	}
}

void QueryManager::makeRecordData(uchar *buf, TableMetaPage const &meta, CommandDef::RecordValue const &value) const {
	if(value.values.size() != meta.columnSize)
		throw ExecuteError("Value attr size not equal to column size");
	for(int i=0; i<meta.columnSize; ++i) {
		auto& col = meta.columns[i];
		parse(value.values[i], buf + col.offset, col.dataType, col.size);
	}
}
