//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXMANAGER_H
#define TEST_INDEXMANAGER_H

#include <string>
#include "IndexHandle.h"

class Database;

namespace Index {

class IndexManager {
	friend class ::Database;
	Database& database;

	IndexManager(Database& database);
	~IndexManager();
public:
	void CreateIndex(std::string tableName, int indexNo);
	void DestroyIndex(std::string tableName, int indexNo);
	IndexHandle OpenIndex(std::string tableName, int indexNo);
	void CloseIndex(IndexHandle const& indexHandle);
};

}

#endif //TEST_INDEXMANAGER_H
