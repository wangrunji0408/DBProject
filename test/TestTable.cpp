//
// Created by 王润基 on 2017/10/16.
//

#include <gtest/gtest.h>
#include <DatabaseManager.h>
#include "TestBase.h"

namespace {

struct Data
{
	int a, b, c;
};

class TestTable : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		dbm->createDatabase("db1");
		dbm->useDatabase("db1");
		db = dbm->getCurrentDatabase();
		db->createTable("table1", sizeof(Data));
		table = db->getTable("table1");
	}

	void Reopen() override {
		TestBase::Reopen();
		dbm->useDatabase("db1");
		db = dbm->getCurrentDatabase();
		table = db->getTable("table1");
	}

	Database* db;
	Table* table;
};

TEST_F(TestTable, GetRecordLength)
{
	ASSERT_EQ(sizeof(Data), table->getRecordLength());
}

TEST_F(TestTable, CanInsertAndGetRecord)
{
	auto data = Data();
	auto rid = table->insertRecord((uchar*)&data);
	auto record = table->getRecord(rid);
	ASSERT_EQ(rid, record.recordID);
	ASSERT_NE((void*)&data, (void*)record.data);
	ASSERT_DATA_EQ(&data, record.data, sizeof(Data));
	Reopen();
	record = table->getRecord(rid);
	ASSERT_DATA_EQ(&data, record.data, sizeof(Data));
}

TEST_F(TestTable, CanDeleteRecord)
{
	auto data = Data();
	auto rid = table->insertRecord((uchar*)&data);
	table->deleteRecord(rid);
	ASSERT_ANY_THROW( table->getRecord(rid) );
	Reopen();
	ASSERT_ANY_THROW( table->getRecord(rid) );
}

TEST_F(TestTable, CanUpdateRecord)
{
	auto data = Data();
	data.a = 0;
	auto rid = table->insertRecord((uchar*)&data);
	auto record = table->getRecord(rid);
	record.getDataRef<Data>().a = 1;
	table->updateRecord(record);
	Reopen();
	record = table->getRecord(rid);
	ASSERT_EQ(1, record.getDataRef<Data>().a);
}

TEST_F(TestTable, CanIterateAll)
{
	for(int i=0; i<10; ++i)
	{
		auto data = Data{i, i+1, i+2};
		table->insertRecord((uchar*)&data);
	}
	auto iter = table->iterateRecords();
	int i = 0;
	for(; iter.hasNext(); i++)
	{
		auto record = iter.getNext();
		auto data = record.getDataRef<Data>();
		ASSERT_EQ(i, data.a);
		ASSERT_EQ(i+1, data.b);
		ASSERT_EQ(i+2, data.c);
	}
	ASSERT_EQ(10, i);
}

}