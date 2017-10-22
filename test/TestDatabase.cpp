//
// Created by 王润基 on 2017/10/23.
//

#include <gtest/gtest.h>
#include <DatabaseManager.h>

namespace {

class TestDatabase : public testing::Test
{
protected:
	virtual void SetUp() {
		dbm = new DatabaseManager();
		dbm->createDatabase("db1");
		dbm->useDatabase("db1");
		db = dbm->getCurrentDatabase();
	}

	DatabaseManager* dbm;
	Database* db;
};

TEST_F(TestDatabase, CanCreateAndGetTable)
{
	db->createTable("table", 10);
	auto table = db->getTable("table");
	ASSERT_NE(nullptr, table);
}

TEST_F(TestDatabase, ReturnNullWhenGetNotExistTable)
{
	ASSERT_EQ(nullptr, db->getTable("table"));
}

TEST_F(TestDatabase, ThrowWhenCreateExistTable)
{
	db->createTable("table", 10);
	auto table = db->getTable("table");
	ASSERT_ANY_THROW( db->createTable("table", 20); );
	ASSERT_EQ(table, db->getTable("table"));
}

TEST_F(TestDatabase, ThrowWhenRecordSizeIsNotPositive)
{
	ASSERT_ANY_THROW( db->createTable("table", 0); );
	ASSERT_ANY_THROW( db->createTable("table", -10); );
	ASSERT_EQ(nullptr, db->getTable("table"));
}

TEST_F(TestDatabase, CanDeleteTable)
{
	db->createTable("table", 10);
	auto table = db->getTable("table");
	db->deleteTable(table);
	ASSERT_EQ(nullptr, db->getTable("table"));
}

TEST_F(TestDatabase, NothingHappensWhenDeleteNullTable)
{
	db->deleteTable(nullptr);
}

}