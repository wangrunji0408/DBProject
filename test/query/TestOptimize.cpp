#include <ast/Command.h>
#include <query/QueryManager.h>
#include <future>
#include "TestQueryBase.h"

namespace {

class TestOptimize : public TestQueryBase
{
protected:
	void SetUp() override {
		TestQueryBase::SetUp();
		insertRecords();
	}

	void Reopen() override {
		TestQueryBase::Reopen();
	}
};

#define GTEST_TIMEOUT_BEGIN auto asyncFuture = std::async(std::launch::async, [this]()->void {
#define GTEST_TIMEOUT_END(X) return; }); \
EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(X)) != std::future_status::timeout);

TEST_F(TestOptimize, Insert_1e5_4s)
{
	const int N = 100000;

	GTEST_TIMEOUT_BEGIN
	auto cmd = Insert();
	cmd.tableName = "book";
	cmd.records.reserve(N);
	for(int i=0; i<N; ++i) {
		auto record = TableRecord();
		record.pushInt(i+10);
		record.pushVarchar("Book" + std::to_string(i));
		cmd.records.push_back(record);
	}
	db->execute(cmd);
	GTEST_TIMEOUT_END(4000)

	ASSERT_EQ(N+2, db->getTable("book")->size());
}

TEST_F(TestOptimize, SearchWithIndex_1e5_1s)
{
	const int N = 100000;
	auto cmd = Insert();
	cmd.tableName = "book";
	cmd.records.reserve(N);
	for(int i=0; i<N; ++i) {
		auto record = TableRecord();
		record.pushInt(i+10);
		record.pushVarchar("Book" + std::to_string(i));
		cmd.records.push_back(record);
	}
	db->execute(cmd);

	GTEST_TIMEOUT_BEGIN
	for(int i=0; i<100000; ++i) {
		int r = rand() % (1 << 16) + 10;
		auto select = Select();
		select.froms = {"book"};
		select.selects = {"id"};
		select.where = {{{"", "id", BoolExpr::OP_EQ, std::to_string(r), ""}}};
		auto result = db->select(select);
		ASSERT_EQ(1, result.records.size());
		ASSERT_EQ(TableRecord().pushInt(r), result.records[0]);
	}
	GTEST_TIMEOUT_END(1000)
}

}