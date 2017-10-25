//
// Created by 王润基 on 2017/10/23.
//

#ifndef TEST_TESTBASE_H
#define TEST_TESTBASE_H

#include <gtest/gtest.h>
#include "systemmanager/DatabaseManager.h"

class TestBase : public testing::Test
{
protected:

	void ClearAllDatabase() {
#ifdef __WIN32__
		system("del *.dbf");
#endif
#ifdef __APPLE__
		system("rm *.dbf");
#endif
	}

	void SetUp() override {
		ClearAllDatabase();
		dbm = new DatabaseManager();
		dbm->createDatabase("db1");
		dbm->useDatabase("db1");
		db = dbm->getCurrentDatabase();
	}

	virtual void Reopen() {
		delete dbm;
		dbm = new DatabaseManager();
		dbm->useDatabase("db1");
		db = dbm->getCurrentDatabase();
	}

	void TearDown () override {
		delete dbm; dbm = nullptr;
	}

	DatabaseManager* dbm = nullptr;
	Database* db = nullptr;
};

inline void ASSERT_DATA_EQ(const void* a, const void* b, size_t size)
{
	ASSERT_NE(nullptr, a);
	ASSERT_NE(nullptr, b);
	char *ca = (char*)a;
	char *cb = (char*)b;
	for(int i=0; i<size; ++i)
		ASSERT_EQ(ca[i], cb[i]);
}

#endif //TEST_TESTBASE_H
