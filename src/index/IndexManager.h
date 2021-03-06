//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXMANAGER_H
#define TEST_INDEXMANAGER_H

#include <string>
#include <ast/TableDef.h>
#include "Index.h"
#include "IndexEntityLists.h"

class Database;

class IndexManager {
	friend class Database;
	friend class Index;
	Database& database;
	Page sysIndexPage;
	IndexEntityLists entityLists;

public: // for test
	IndexEntityLists* getEntityLists();

private:
	void resetRootPageID(int indexID, int pageID);
	IndexManager(Database& database);
public:
	~IndexManager() = default;
	int createIndex(int tablePageID, DataType keyType, short keyLength);
	void deleteIndex(int indexID);
	std::unique_ptr<Index> getIndex(int indexID);
};

#endif //TEST_INDEXMANAGER_H
