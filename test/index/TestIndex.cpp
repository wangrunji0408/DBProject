//
// Created by 王润基 on 2017/10/23.
//

#include "../TestBase.h"
#include "indexmanager/IndexPage.h"

namespace {

class TestIndex : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
	}

	void Reopen () override {
		TestBase::Reopen();
	}
};

TEST_F(TestIndex, SizeofIndexPage)
{
	ASSERT_EQ(48, sizeof(std::function<bool(const void *, const void *)>));
	ASSERT_EQ(8192, sizeof(IndexPage));
}

}