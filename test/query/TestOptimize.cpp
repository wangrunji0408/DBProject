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
	}

	void Reopen() override {
		TestQueryBase::Reopen();
	}

	void insertBooks(int N) {
		auto cmd = Insert();
		cmd.tableName = "book";
		cmd.records.reserve(N);
		for(int i=0; i<N; ++i) {
			auto record = TableRecord();
			record.pushInt(i);
			record.pushVarchar("Book" + std::to_string(i));

			cmd.records.push_back(record);
		}
		db->execute(cmd);
	}

	void insertPeoples(int N) {
		auto cmd = Insert();
		cmd.tableName = "people";
		cmd.records.reserve(N);
		for(int i=0; i<N; ++i) {
			auto record = TableRecord::fromString(types,
				{std::to_string(i),"People" + std::to_string(i),"M","","","",""});
			cmd.records.push_back(record);
		}
		db->execute(cmd);
	}

	void insertBorrows(int N) {
		auto cmd = Insert();
		cmd.tableName = "borrow";
		cmd.records.reserve(N);
		for(int i=0; i<N; ++i) {
			auto record = TableRecord();
			record.pushInt(i);
			record.pushInt(i);
			cmd.records.push_back(record);
		}
		db->execute(cmd);
	}
};

#define GTEST_TIMEOUT_BEGIN auto asyncFuture = std::async(std::launch::async, [this]()->void {
#define GTEST_TIMEOUT_END(X) return; }); \
EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(X)) != std::future_status::timeout);

TEST_F(TestOptimize, Insert_1e4_100ms)
{
	const int N = 10000;

	GTEST_TIMEOUT_BEGIN
	insertBooks(N);
	GTEST_TIMEOUT_END(100)

	ASSERT_EQ(N, db->getTable("book")->size());
}

TEST_F(TestOptimize, Update_1e4_10ms)
{
	const int N = 10000;
	insertBooks(N);

	GTEST_TIMEOUT_BEGIN
	auto cmd = Update();
	cmd.tableName = "book";
	cmd.sets = {{"name", "Name"}};
	db->execute(cmd);
	GTEST_TIMEOUT_END(10)
}

TEST_F(TestOptimize, SelectWithIndex_1e5_100ms)
{
	const int N = 1 << 14;
	insertBooks(N);

	GTEST_TIMEOUT_BEGIN
	for(int i=0; i<100000; ++i) {
		int r = rand() % N;
		auto select = Select();
		select.froms = {"book"};
		select.selects = {"id"};
		select.where = {{{"", "id", BoolExpr::OP_EQ, std::to_string(r), ""}}};
		auto result = db->select(select);
		ASSERT_EQ(1, result.records.size());
		ASSERT_EQ(TableRecord().pushInt(r), result.records[0]);
	}
	GTEST_TIMEOUT_END(100)
}

TEST_F(TestOptimize, UpdateWithIndex_1e5_100ms)
{
	const int N = 1 << 14;
	insertBooks(N);

	GTEST_TIMEOUT_BEGIN
	for(int i=0; i<100000; ++i) {
		int r = rand() % N;
		auto cmd = Update();
		cmd.tableName = "book";
		cmd.sets = {{"name", "Name"}};
		cmd.where = {{{"", "id", BoolExpr::OP_EQ, std::to_string(r), ""}}};
		db->execute(cmd);
	}
	GTEST_TIMEOUT_END(100)
}

TEST_F(TestOptimize, SelectWithForeignKey_1e4_150ms)
{
	const int N = 10000;
	insertBooks(N);
	insertPeoples(N);
	insertBorrows(N);

	GTEST_TIMEOUT_BEGIN
	auto select = Select();
	select.froms = {"book", "borrow"};
	select.selects = {"book.id"};
	select.where = {{
		{"book", "id", BoolExpr::OP_EQ, "", "borrow.book_id"},
//		{"borrow", "people_id", BoolExpr::OP_EQ, "1", ""},
	}};
	auto result = db->select(select);
	ASSERT_EQ(10000, result.records.size());
	GTEST_TIMEOUT_END(150)
}

}