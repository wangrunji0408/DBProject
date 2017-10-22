//
// Created by 王润基 on 2017/10/16.
//

#include <gtest/gtest.h>
#include <DatabaseManager.h>
#include <Table.h>

namespace {

class TestTable : public testing::Test
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

struct Data
{
	int a, b, c;
};

TEST_F(TestTable, SampleTest)
{
	db->createTable("table1", sizeof(Data));
	auto table = db->getTable("table1");
	auto data = new Data();
	table->insertRecord((BufType)data);

}

}