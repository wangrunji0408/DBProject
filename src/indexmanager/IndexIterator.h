//
// Created by 王润基 on 2017/10/26.
//

#ifndef TEST_INDEXITERATOR_H
#define TEST_INDEXITERATOR_H


#include <memory>
#include <filesystem/page/Page.h>

class Database;

class IndexIterator {
	friend class Index;

	bool end = false;
	int slotID;
	Page page;
	Database& database;

	IndexIterator(Database& database, Page page, int slotID);
public:
	~IndexIterator() = default;
	bool hasNext() const;
	void* getNext();
};


#endif //TEST_INDEXITERATOR_H
