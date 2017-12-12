//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_RECORDMANAGER_H
#define TEST_RECORDMANAGER_H

#include <string>
#include <memory>
#include <record/Table.h>
#include <ast/TableDef.h>

class Database;
class DatabaseManager;
class Table;

class RecordManager {

	friend class Table;
	friend class RecordScanner;
	friend class TableMetaPage;

	static const int MAX_TABLE_COUNT = 30;

	::std::unique_ptr<Table> tables[MAX_TABLE_COUNT];
	size_t tableCount;

	void recoverTables();

	friend class ::Database;
	Database& database;

	RecordManager(Database& database);
	Table* getTable(int id) const;
public:
	~RecordManager();
	void createTable(::std::string name,size_t recordLength);
	void deleteTable(Table* table);
	Table* getTable(::std::string name);
};

#endif //TEST_RECORDMANAGER_H
