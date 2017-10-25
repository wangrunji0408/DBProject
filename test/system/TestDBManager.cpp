//
// Created by 王润基 on 2017/10/16.
//

#include <gtest/gtest.h>
#include <systemmanager/DatabaseManager.h>
#include "../TestBase.h"

namespace {

class TestDBManager : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		try {
			dbm->createDatabase("db1");
			dbm->useDatabase("db1");
		} catch (std::exception const& e) {}
	}

	void Reopen() override {
		TestBase::Reopen();
		dbm->useDatabase("db1");
	}
};

TEST_F(TestDBManager, ShouldBeSingleton)
{
	ASSERT_ANY_THROW ( new DatabaseManager() );
}

TEST_F(TestDBManager, CanCreateAndGetCurrent)
{
	ASSERT_NE(nullptr, dbm->getCurrentDatabase());
	Reopen();
	ASSERT_NE(nullptr, dbm->getCurrentDatabase());
}

TEST_F(TestDBManager, ThrowWhenCreateNameExist)
{
	ASSERT_ANY_THROW( dbm->createDatabase("db1"); );
}

TEST_F(TestDBManager, ThrowWhenGetNotExist)
{
	ASSERT_ANY_THROW( dbm->useDatabase("db2") );
}

TEST_F(TestDBManager, CanDelete)
{
	dbm->deleteCurrentDatabase();
	ASSERT_ANY_THROW( dbm->getCurrentDatabase() );
	ASSERT_ANY_THROW( dbm->useDatabase("db1") );
	TestBase::Reopen();
	ASSERT_ANY_THROW( dbm->useDatabase("db1") );
}

}