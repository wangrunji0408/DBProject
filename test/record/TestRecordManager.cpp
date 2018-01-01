//
// Created by 王润基 on 2017/10/23.
//

#include <gtest/gtest.h>
#include <system/DatabaseManager.h>
#include "../TestBase.h"

namespace {

class TestRecordManager : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		rm = db->getRecordManager();
	}

	void Reopen () override {
		TestBase::Reopen();
		rm = db->getRecordManager();
	}

	RecordManager* rm;
};

TEST_F(TestRecordManager, CanCreateAndGetSet)
{
	rm->createSet("table", 10);
	ASSERT_NE(nullptr, rm->getSet("table"));
	Reopen();
	ASSERT_NE(nullptr, rm->getSet("table"));
}

TEST_F(TestRecordManager, CanGetSetNames)
{
	rm->createSet("nyan", 10);
	rm->createSet("nyanyan", 10);
	auto list=rm->getSetNames();
	ASSERT_EQ(3,list.size());
	bool hasIEL=false;
	bool hasNyan=false;
	bool hasNyanyan=false;
	for(auto name:list){
		if(name=="**IndexEntityList**"){//should not use const here
			hasIEL=true;
		}
		if(name=="nyan"){
			hasNyan=true;
		}
		if(name=="nyanyan"){
			hasNyanyan=true;
		}
	}
	ASSERT_EQ(true,hasIEL);
	ASSERT_EQ(true,hasNyan);
	ASSERT_EQ(true,hasNyanyan);
}

TEST_F(TestRecordManager, ThrowWhenGetNotExistSet)
{
	ASSERT_ANY_THROW(rm->getSet("table"));
}

TEST_F(TestRecordManager, ThrowWhenCreateExistSet)
{
	rm->createSet("table", 10);
	auto table = rm->getSet("table");
	ASSERT_ANY_THROW(rm->createSet("table", 20); );
	ASSERT_EQ(table, rm->getSet("table"));
}

TEST_F(TestRecordManager, ThrowWhenRecordSizeIsInvalid)
{
	ASSERT_ANY_THROW(rm->createSet("table", 0); );
	ASSERT_ANY_THROW(rm->createSet("table", -10); );
	ASSERT_ANY_THROW(rm->createSet("table", 8096); );
	ASSERT_ANY_THROW(rm->getSet("table") );
	ASSERT_NO_THROW(rm->createSet("table", 8095); );
}

TEST_F(TestRecordManager, ThrowWhenSetNameTooLong)
{
	std::string name;
	name.resize(125, 'a');
	ASSERT_ANY_THROW(rm->createSet(name, 10); );

	name.resize(124, 'a');
	ASSERT_NO_THROW(rm->createSet(name, 10); );
}

TEST_F(TestRecordManager, ThrowWhenSetCountExceed30)
{
	// 1 for IndexEntityListTable
	for(int i=0; i<30-1; ++i)
		rm->createSet("table" + to_string(i), 10);
	ASSERT_ANY_THROW(rm->createSet("the_final_straw", 10); );
}

TEST_F(TestRecordManager, CanDeleteSet)
{
	rm->createSet("table", 10);
	rm->deleteSet("table");
	ASSERT_ANY_THROW(rm->getSet("table") );
}

}