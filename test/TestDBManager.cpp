//
// Created by 王润基 on 2017/10/16.
//

#include <gtest/gtest.h>
#include <DatabaseManager.h>

namespace {

class TestDBManager : public testing::Test
{
protected:
	virtual void SetUp() {
		dbm = new DatabaseManager();
		dbm->createDatabase("db1");
		dbm->useDatabase("db1");
	}

	DatabaseManager* dbm;
};

TEST_F(TestDBManager, CanGetCurrent)
{
	auto db = dbm->getCurrentDatabase();
	ASSERT_NE(db, nullptr);
}

TEST_F(TestDBManager, ThrowWhenGetNotExist)
{
	ASSERT_ANY_THROW( dbm->useDatabase("db2") );
}

TEST_F(TestDBManager, CanDelete)
{
	dbm->deleteCurrentDatabase();
	ASSERT_ANY_THROW( dbm->useDatabase("db1") );
}



}