//
// Created by 王润基 on 2017/10/23.
//

#include <random>
#include "../TestBase.h"
#include "indexmanager/IndexPage.h"

namespace {

class TestIndex : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		IndexPage::TEST_MODE = true; // MUST set before create index
		im = db->getIndexManager();
		indexID = im->createIndex(0, INT, 4);
		index = im->getIndex(indexID);
	}

	void Reopen () override {
		TestBase::Reopen();
		im = db->getIndexManager();
		index = im->getIndex(indexID);
	}

	void TearDown() override {
		TestBase::TearDown();
		IndexPage::TEST_MODE = false;
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
	const int n = 100;
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;
	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n; ++i)
	{
		index->insertEntry(data + i, RID(data[i], data[i]));
//		index->print();
	}
	for(int i=0; i<n; ++i)
		ASSERT_EQ(RID(i, i), index->findEntry(new int(i)));
	Reopen();
	for(int i=0; i<n; ++i)
		ASSERT_EQ(RID(i, i), index->findEntry(new int(i)));
}

TEST_F(TestIndex, CanDelete)
{
	const int n = 10;
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;

	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n; ++i)
		index->insertEntry(data + i, RID(data[i], data[i]));
	index->print();

	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n; ++i)
	{
		index->deleteEntry(data + i, RID(data[i], data[i]));
//		std::cerr << "delete " << data[i] << ": ";
//		index->print();
		ASSERT_ANY_THROW( index->findEntry(data + i) );
		if(i != n-1)
			ASSERT_EQ(RID(data[i+1], data[i+1]), index->findEntry(data+i+1));
	}
}

TEST_F(TestIndex, RandomInsertDeleteFind)
{
	const int n = 10;
	bool in[n] = {0};
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;
	for(int k=0; k<10000; ++k)
	{
		int i = rand() % n;
		if(in[i])
		{
			ASSERT_EQ(RID(i, i), index->findEntry(&data[i]));
			index->deleteEntry(&data[i], RID(i,i));
			in[i] = false;
		}
		else
		{
			ASSERT_ANY_THROW( index->findEntry(&data[i]) );
			index->insertEntry(&data[i], RID(i,i));
			in[i] = true;
		}
	}
}

TEST_F(TestIndex, CanIterate)
{
	const int n = 100;
	int data[n];
	for(int i=0; i<n; ++i)
		data[i] = i;
	std::shuffle(data, data + n, std::default_random_engine());
	for(int i=0; i<n; ++i)
		index->insertEntry(data + i, RID(data[i], data[i]));
	auto iter = index->getIterator();
	for(int i=0; i<n; ++i)
	{
		ASSERT_TRUE(iter.hasNext());
		ASSERT_EQ(i, *(int*)iter.getNext());
	}
	ASSERT_FALSE(iter.hasNext());
}

TEST_F(TestIndex, ThrowWhenTryToModifyDuringIterating)
{
	{
		auto iter = index->getIterator();
		ASSERT_ANY_THROW( index->insertEntry(new int(1), RID(1,1)) );
		ASSERT_ANY_THROW( index->deleteEntry(new int(1), RID(1,1)) );
	}
	index->insertEntry(new int(1), RID(1,1));
}

}