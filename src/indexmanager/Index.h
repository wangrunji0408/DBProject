//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXHANDLE_H
#define TEST_INDEXHANDLE_H

#include <filesystem/page/Page.h>
#include "IndexPage.h"
#include "IndexIterator.h"

class RID;
class Database;

class Index {
	friend class IndexManager;
	int rootPageID;
	Database& database;
	IndexPage::TCompare compare;

	// temp for insert
	bool needSplit;
	int otherPageID;
	const void* midKey;

	Index(Database& database, int rootPageID);
	RID findEntryIndexPos(const void *pData);
	void insertEntry(const void *pData, RID const& rid, int nodePageID);
public:
	~Index() = default;
	void insertEntry(const void *pData, RID const& rid);
	void deleteEntry(const void *pData, RID const& rid);
	RID findEntry(const void *pData);
	IndexIterator getIterator() const;
	void print(int pageID = 0);
};

#endif //TEST_INDEXHANDLE_H
