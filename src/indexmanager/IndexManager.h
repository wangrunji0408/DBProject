//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXMANAGER_H
#define TEST_INDEXMANAGER_H

#include <string>
#include <ast/TableDef.h>
#include "Index.h"

class Database;

class IndexManager {
	friend class Database;
	friend class Index;
	Database& database;
	Page sysIndexPage;

	void resetRootPageID(int indexID, int pageID);
	IndexManager(Database& database);
public:
	~IndexManager() = default;
	int createIndex(int tablePageID, DataType keyType, short keyLength);
	void deleteIndex(int indexID);
	Index* getIndex(int indexID); // TODO 内存管理
};

#endif //TEST_INDEXMANAGER_H
