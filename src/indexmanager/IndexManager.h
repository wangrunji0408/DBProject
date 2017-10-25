//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXMANAGER_H
#define TEST_INDEXMANAGER_H

#include <string>
#include "Index.h"

class Database;

class IndexManager {
	friend class Database;
	Database& database;

	IndexManager(Database& database);
public:
	~IndexManager();
	void createIndex(std::string tableName, int indexNo);
	void destroyIndex(std::string tableName, int indexNo);
	Index getIndex(std::string tableName, int indexNo);
};

#endif //TEST_INDEXMANAGER_H
