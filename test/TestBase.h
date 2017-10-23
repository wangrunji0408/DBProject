//
// Created by 王润基 on 2017/10/23.
//

#ifndef TEST_TESTBASE_H
#define TEST_TESTBASE_H


#include <gtest/gtest.h>

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
};


#endif //TEST_TESTBASE_H