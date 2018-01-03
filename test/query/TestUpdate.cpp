#include <ast/Command.h>
#include <query/QueryManager.h>
#include <ast/Exceptions.h>
#include "TestQueryBase.h"

namespace {

class TestUpdate : public TestQueryBase
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

TEST_F(TestUpdate, ThrowWhenNameNotExist)
{
	auto cmd = Update();
	cmd.tableName = "people";
	cmd.sets = {{"hehe", "hehe"}};
	ASSERT_THROW( db->execute(cmd), NameNotExistError );

	cmd.sets = {};
	cmd.where = {{{"", "hehe", BoolExpr::OP_EQ, "1", ""},}};
	ASSERT_THROW( db->execute(cmd), NameNotExistError );
}

TEST_F(TestUpdate, ThrowWhenIsNull)
{
	auto cmd = Update();
	cmd.tableName = "people";
	cmd.sets = {{"name", ""}};
	cmd.where = {{{"", "id", BoolExpr::OP_EQ, "1", ""},}};
	ASSERT_THROW( db->execute(cmd), NullValueError );
}

TEST_F(TestUpdate, ThrowWhenForeignKeyNotExist)
{
	auto cmd = Update();
	cmd.tableName = "borrow";
	cmd.sets = {{"book_id", "3"}};
	cmd.where = {{{"", "people_id", BoolExpr::OP_EQ, "1", ""},}};
	ASSERT_THROW( db->execute(cmd), ForeignKeyNotExistError );
}

}