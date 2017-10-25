//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXHANDLE_H
#define TEST_INDEXHANDLE_H

#include <filesystem/page/Page.h>

class RID;
class Database;

class Index {
	friend class IndexManager;
	Page rootPage;
	Database& database;

	Index(Database& database, int rootPageID);
	RID findEntryIndexPos(const void *pData);
public:
	~Index() = default;
	void insertEntry(const void *pData, RID const& rid);
	void deleteEntry(const void *pData, RID const& rid);
	RID findEntry(const void *pData);
};

#endif //TEST_INDEXHANDLE_H
