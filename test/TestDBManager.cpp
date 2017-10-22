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

	}

};

TEST_F(TestDBManager, SampleTest)
{
	auto dbm = new DatabaseManager();
	dbm->createDatabase("db1");
	dbm->useDatabase("db1");
	auto db = dbm->getCurrentDatabase();
	ASSERT_NE(db, nullptr);

	dbm->useDatabase("db2");
	ASSERT_EQ(dbm->getCurrentDatabase(), nullptr);
}

}