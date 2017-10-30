//
// Created by 王润基 on 2017/10/23.
//

#include <gtest/gtest.h>
#include <systemmanager/DatabaseManager.h>
#include "../TestBase.h"

namespace {

class TestDatabase : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
	}

	void Reopen () override {
		TestBase::Reopen();
	}
};

TEST_F(TestDatabase, CanCreateAndGetTable)
{
	db->createTable("table", 10);
	ASSERT_NE(nullptr, db->getTable("table"));
	Reopen();
	ASSERT_NE(nullptr, db->getTable("table"));
}

TEST_F(TestDatabase, ThrowWhenGetNotExistTable)
{
	ASSERT_ANY_THROW(db->getTable("table"));
}

TEST_F(TestDatabase, ThrowWhenCreateExistTable)
{
	db->createTable("table", 10);
	auto table = db->getTable("table");
	ASSERT_ANY_THROW( db->createTable("table", 20); );
	ASSERT_EQ(table, db->getTable("table"));
}

TEST_F(TestDatabase, ThrowWhenRecordSizeIsInvalid)
{
	ASSERT_ANY_THROW( db->createTable("table", 0); );
	ASSERT_ANY_THROW( db->createTable("table", -10); );
	ASSERT_ANY_THROW( db->createTable("table", 8096); );
	ASSERT_ANY_THROW( db->getTable("table") );
	ASSERT_NO_THROW( db->createTable("table", 8095); );
}

TEST_F(TestDatabase, ThrowWhenTableNameTooLong)
{
	std::string name;
	name.resize(125, 'a');
	ASSERT_ANY_THROW( db->createTable(name, 10); );

	name.resize(124, 'a');
	ASSERT_NO_THROW( db->createTable(name, 10); );
}

TEST_F(TestDatabase, ThrowWhenTableCountExceed30)
{
	// 1 for IndexEntityListTable
	for(int i=0; i<30-1; ++i)
		db->createTable("table" + to_string(i), 10);
	ASSERT_ANY_THROW( db->createTable("the_final_straw", 10); );
}

TEST_F(TestDatabase, CanDeleteTable)
{
	db->createTable("table", 10);
	auto table = db->getTable("table");
	db->deleteTable(table);
	ASSERT_ANY_THROW( db->getTable("table") );
}

TEST_F(TestDatabase, NothingHappensWhenDeleteNullTable)
{
	db->deleteTable(nullptr);
}

TEST_F(TestDatabase, CanAcquireAndGetPage)
{
	auto page1 = db->acquireNewPage();
	ASSERT_EQ(page1, db->getPage(page1.pageId));
	ASSERT_TRUE(db->isPageUsed(page1.pageId));
	ASSERT_FALSE(db->isPageUsed(100));
}

TEST_F(TestDatabase, CanKeepPageData)
{
	int pageId;
	uchar data[8192];
	{
		auto page1 = db->acquireNewPage();
		pageId = page1.pageId;
		std::memcpy(page1.getDataMutable(), data, 8192);
	}
	Reopen();
	{
		auto page1 = db->getPage(pageId);
		ASSERT_DATA_EQ(page1.getDataReadonly(), data, 8192);
	}
}

TEST_F(TestDatabase, CanReleasePage)
{
	auto page1 = db->acquireNewPage();
	ASSERT_TRUE(db->isPageUsed(page1.pageId));
	db->releasePage(page1.pageId);
	ASSERT_FALSE(db->isPageUsed(page1.pageId));
	ASSERT_ANY_THROW( db->releasePage(100) );
}

}