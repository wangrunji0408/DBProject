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
class IndexManager;
class IndexEntityLists;

class Index {
	friend class IndexManager;
	friend class IndexIterator;
	int id;
	int rootPageID;
	Database& database;
	IndexEntityLists& entityLists;
	IndexPage::TCompare compare;
	int iteratorCount = 0;

	// temp for insert
	bool needSplit;
	int otherPageID;
	const void* midKey;
	// temp for delete
	bool needMerge;
	const void* minKey;
	// temp
	char* keyridBuf;
	int keyLength;

	Index(IndexManager &manager, int rootPageID);
	void loadToBuf(const void *pData, RID const& rid) const;
	RID lowerBound(const void *pData, RID const& rid) const;
	bool equals(const void* data1, const void* data2) const;
	void insertEntry(int nodePageID);
	void deleteEntry(int nodePageID);
	void fixDelete(IndexPage* node, int leftPos);
	void updateRoot(int rootPageID);
public:
	~Index();
	void insertEntry(const void *pData, RID const& rid);
	void deleteEntry(const void *pData, RID const& rid);
	bool containsEntry(const void *pData, RID const& rid) const;
	bool containsEntry(const void *pData) const;
	IndexIterator begin() const;
	IndexIterator lowerBound(const void *pData) const;
	IndexIterator upperBound(const void *pData) const;
	void print(int pageID = 0) const;
};

#endif //TEST_INDEXHANDLE_H
