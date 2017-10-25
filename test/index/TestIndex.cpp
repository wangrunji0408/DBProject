//
// Created by 王润基 on 2017/10/23.
//

#include "../TestBase.h"
#include "indexmanager/IndexPage.h"

namespace {

class TestIndex : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		im = db->getIndexManager();
		indexID = im->createIndex(0, INT, 4);
		index = im->getIndex(indexID);
	}

	void Reopen () override {
		TestBase::Reopen();
		im = db->getIndexManager();
		index = im->getIndex(indexID);
	}
	IndexManager* im = nullptr;
	Index* index = nullptr;
	int indexID;
};

TEST_F(TestIndex, SizeofIndexPage)
{
	ASSERT_EQ(8192, sizeof(IndexPage));
}

TEST_F(TestIndex, CanInsertAndFindEntry)
{
	const int n = 1000;
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;
	random_shuffle(data, data + n);
	for(int i=0; i<n; ++i)
		index->insertEntry(data + i, RID(data[i], data[i]));
	for(int i=0; i<n; ++i)
		ASSERT_EQ(RID(i, i), index->findEntry(new int(i)));
}

}