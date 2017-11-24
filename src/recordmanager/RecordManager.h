//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_RECORDMANAGER_H
#define TEST_RECORDMANAGER_H

#include <string>
#include <memory>
#include <recordmanager/Table.h>

class Database;
class DatabaseManager;
class Table;

class RecordManager {

	friend class Table;
	friend class RecordScanner;

	::std::unique_ptr<Table> tables[30];
	size_t tableCount;

	void recoverTables();

	friend class ::Database;
	Database& database;

	RecordManager(Database& database);
public:
	~RecordManager();
	void createTable(::std::string name,size_t recordLength);
	void deleteTable(Table* table);
	Table* getTable(::std::string name);
};

#endif //TEST_RECORDMANAGER_H
