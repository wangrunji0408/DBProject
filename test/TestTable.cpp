//
// Created by 王润基 on 2017/10/16.
//

#include <gtest/gtest.h>
#include <DatabaseManager.h>
#include <Table.h>
#include <exception>

namespace {

struct Data
{
	int a, b, c;
};

inline void ASSERT_DATA_EQ(void* a, void* b, size_t size)
{
	ASSERT_NE(nullptr, a);
	ASSERT_NE(nullptr, b);
	char *ca = (char*)a;
	char *cb = (char*)b;
	for(int i=0; i<size; ++i)
		ASSERT_EQ(ca[i], cb[i]);
}

class TestTable : public testing::Test
{
protected:
	virtual void SetUp() {
		system("rm *.dbf");
		dbm = DatabaseManager();
		dbm.createDatabase("db1");
		dbm.useDatabase("db1");
		db = dbm.getCurrentDatabase();
		db->createTable("table1", sizeof(Data));
		table = db->getTable("table1");
	}

	DatabaseManager dbm;
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
	auto rid = table->insertRecord((BufType)&data);
	auto record = table->getRecord(rid);
	ASSERT_EQ(rid, record.recordID);
	ASSERT_NE((void*)&data, (void*)record.data);
	ASSERT_DATA_EQ(&data, record.data, sizeof(Data));
}

TEST_F(TestTable, CanDeleteRecord)
{
	auto data = Data();
	auto rid = table->insertRecord((BufType)&data);
	table->deleteRecord(rid);
	ASSERT_ANY_THROW( table->getRecord(rid) );
}

TEST_F(TestTable, CanUpdateRecord)
{
	throw ::std::runtime_error("Not Implemented.");
	auto data = Data();
	data.a = 0;
	auto rid = table->insertRecord((BufType)&data);
	auto record = table->getRecord(rid);
	auto dataInTable = record.getDataRef<Data>();
	dataInTable.a = 1;
	table->updateRecord(record);
}

TEST_F(TestTable, CanIterateAll)
{
	for(int i=0; i<10; ++i)
	{
		auto data = Data{i, i+1, i+2};
		table->insertRecord((BufType)&data);
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

TEST_F(TestTable, CanIterateByFilter)
{
	for(int i=0; i<10; ++i)
	{
		auto data = Data{i, i+1, i+2};
		table->insertRecord((BufType)&data);
	}
	auto iter = table->iterateRecords([](const Record& record){
		return record.getDataRef<Data>().a == 5;
	});
	ASSERT_TRUE(iter.hasNext());
	auto data = iter.getNext().getDataRef<Data>();
	ASSERT_EQ(5, data.a);
	ASSERT_FALSE(iter.hasNext());
}

}