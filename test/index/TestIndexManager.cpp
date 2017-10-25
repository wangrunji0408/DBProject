//
// Created by 王润基 on 2017/10/23.
//

#include "../TestBase.h"
#include "indexmanager/IndexPage.h"

namespace {

class TestIndexManager : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		im = db->getIndexManager();
	}

	void Reopen () override {
		TestBase::Reopen();
		im = db->getIndexManager();
	}
	IndexManager* im = nullptr;
};

TEST_F(TestIndexManager, CanCreateAndGetIndex)
{
	int indexID = im->createIndex(0, INT, 4);
	auto index = im->getIndex(indexID);
	Reopen();
	index = im->getIndex(indexID);
}

TEST_F(TestIndexManager, ThrowWhenCreateWithInvalidArgument)
{
	ASSERT_ANY_THROW( im->createIndex(0, INT, 0) );
	ASSERT_ANY_THROW( im->createIndex(0, INT, -10) );
	ASSERT_ANY_THROW( im->createIndex(0, INT, 8001) );
	ASSERT_ANY_THROW( im->createIndex(0, UNKNOWN, 4) );
}

TEST_F(TestIndexManager, CanDeleteIndex)
{
	int indexID = im->createIndex(0, INT, 4);
	im->deleteIndex(indexID);
	ASSERT_ANY_THROW( im->getIndex(indexID) );
	Reopen();
	ASSERT_ANY_THROW( im->getIndex(indexID) );
}

}