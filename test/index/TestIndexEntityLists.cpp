//
// Created by 王润基 on 2017/10/30.
//

#include <random>
#include "../TestBase.h"
#include "indexmanager/IndexEntityLists.h"

namespace {

class TestIndexEntityLists : public TestBase {
protected:
	void SetUp() override {
		TestBase::SetUp();
		im = db->getIndexManager();
		el = im->getEntityLists();
	}

	void Reopen() override {
		TestBase::Reopen();
		im = db->getIndexManager();
		el = im->getEntityLists();
	}

	void TearDown() override {
		TestBase::TearDown();
	}

	IndexManager *im = nullptr;
	IndexEntityLists* el = nullptr;
};

TEST_F(TestIndexEntityLists, CanCreateList) {
	el->createList();
}

TEST_F(TestIndexEntityLists, CanDeleteList) {
	auto head = el->createList();
	el->deleteList(head);
}

TEST_F(TestIndexEntityLists, CanInsertAndFindEntity) {
	auto head = el->createList();
	const int n = 1000;
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;
	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n; ++i)
		el->insertEntity(RID(data[i],data[i]), head);
	for(int i=0; i<n; ++i)
		ASSERT_TRUE(el->containsEntity(RID(i,i), head));
}

TEST_F(TestIndexEntityLists, CanRemoveEntity) {
	auto head = el->createList();
	const int n = 1000;
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;
	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n; ++i)
		el->insertEntity(RID(data[i],data[i]), head);
	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n/2; ++i)
	{
		el->removeEntity(RID(data[i],data[i]), head);
		ASSERT_FALSE(el->containsEntity(RID(data[i],data[i]), head));
	}
	for(int i=n/2; i<n; ++i)
		ASSERT_TRUE(el->containsEntity(RID(data[i],data[i]), head));
}

}

