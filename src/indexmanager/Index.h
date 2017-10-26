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
	friend class IndexIterator;
	int rootPageID;
	Database& database;
	IndexPage::TCompare compare;
	int iteratorCount = 0;

	// temp for insert
	bool needSplit;
	int otherPageID;
	const void* midKey;
	// temp for delete
	const void* minKey;

	Index(Database& database, int rootPageID);
	RID findEntryIndexPos(const void *pData) const;
	void insertEntry(const void *pData, RID const& rid, int nodePageID);
	void deleteEntry(const void *pData, RID const& rid, int nodePageID);
public:
	~Index() = default;
	void insertEntry(const void *pData, RID const& rid);
	void deleteEntry(const void *pData, RID const& rid);
	RID findEntry(const void *pData) const;
	IndexIterator getIterator() const;
	void print(int pageID = 0) const;
};

#endif //TEST_INDEXHANDLE_H
