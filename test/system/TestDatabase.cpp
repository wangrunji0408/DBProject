//
// Created by 王润基 on 2017/10/23.
//

#include <gtest/gtest.h>
#include <system/DatabaseManager.h>
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