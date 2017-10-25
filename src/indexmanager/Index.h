//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXHANDLE_H
#define TEST_INDEXHANDLE_H

#include <Page.h>

class RID;
class Database;

class Index {
	friend class IndexManager;
	Page rootPage;
	Database& database;

	Index(Database& database, int rootPageID);
public:
	~Index() = default;
	void insertEntry(void *pData, RID const& rid);
	void deleteEntry(void *pData, RID const& rid);
};

#endif //TEST_INDEXHANDLE_H
