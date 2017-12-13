//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_RECORDMANAGER_H
#define TEST_RECORDMANAGER_H

#include <string>
#include <memory>
#include <record/RecordSet.h>
#include <ast/TableDef.h>

class Database;
class DatabaseManager;
class RecordSet;

class RecordManager {

	friend class RecordSet;
	friend class RecordScanner;
	friend class TableMetaPage;

	static const int MAX_TABLE_COUNT = 30;

	::std::unique_ptr<RecordSet> tables[MAX_TABLE_COUNT];
	size_t tableCount;

	void recoverTables();

	friend class ::Database;
	Database& database;

	RecordManager(Database& database);
	RecordSet* getSet(int id) const;
public:
	~RecordManager();
	void createSet(::std::string name, size_t recordLength);
	void deleteSet(::std::string name);
	RecordSet* getSet(::std::string name);
};

#endif //TEST_RECORDMANAGER_H
