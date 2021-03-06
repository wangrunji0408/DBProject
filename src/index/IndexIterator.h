//
// Created by 王润基 on 2017/10/26.
//

#ifndef TEST_INDEXITERATOR_H
#define TEST_INDEXITERATOR_H


#include <memory>
#include <filesystem/page/Page.h>
#include <record/RID.h>

class Index;

class IndexIterator {
	friend class Index;

	bool end = false;
	int slotID;
	Page page;
	Index& index;

	IndexIterator(Index& index, Page page, int slotID);
public:
	IndexIterator(IndexIterator const& it);
	~IndexIterator();
	bool moveNext();
	void* getKey();
	RID getRID();
	bool isEnd() const;
	friend bool operator==(const IndexIterator &lhs, const IndexIterator &rhs);
	friend bool operator!=(const IndexIterator &lhs, const IndexIterator &rhs);
};


#endif //TEST_INDEXITERATOR_H
