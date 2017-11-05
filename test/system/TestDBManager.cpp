//
// Created by 王润基 on 2017/10/16.
//

#include <gtest/gtest.h>
#include <systemmanager/DatabaseManager.h>
#include "../TestBase.h"
#include "systemmanager/DatabaseMetaPage.h"


namespace {

class TestDBManager : public TestBase
{
protected:
	void SetUp() override {
		ClearAllDatabase();
		dbm = new DatabaseManager();
		try {
			dbm->createDatabase("db1");
			dbm->useDatabase("db1");
		} catch (std::exception const& e) {}
	}

	void Reopen() override {
		delete dbm;
		dbm = new DatabaseManager();
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
	delete dbm;
	dbm = new DatabaseManager();
	ASSERT_ANY_THROW( dbm->useDatabase("db1") );
}

TEST_F(TestDBManager, StructureOfDatabaseMetaPage)
{
	ASSERT_EQ(8192, sizeof(DatabaseMetaPage));

	auto meta = DatabaseMetaPage();
#define OFFSET(attr) ((char*)&meta.attr - (char*)&meta)
	ASSERT_EQ(0, OFFSET(magicValue));
	ASSERT_EQ(252, OFFSET(tableCount));
	ASSERT_EQ(256, OFFSET(tableInfo));
	ASSERT_EQ(256, OFFSET(tableInfo[0].metaPageID));
	ASSERT_EQ(260, OFFSET(tableInfo[0].name));
	ASSERT_EQ(256+128, OFFSET(tableInfo[1]));
	ASSERT_EQ(4096, OFFSET(pageUsedBitset));
#undef OFFSET
}

}