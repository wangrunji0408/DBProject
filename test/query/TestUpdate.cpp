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

TEST_F(TestUpdate, Normal)
{
	auto cmd = Update();
	cmd.tableName = "people";
	cmd.sets = {
			{"name", "NewName"},
			{"birthday", "1997/04/03"},
			{"height", "111.1"},
			{"age", "50"},
	};
	cmd.where = {{{"", "id", BoolExpr::OP_EQ, "1", ""},}};
	db->execute(cmd);

	auto select = Select();
	select.froms = {"people"};
	select.selects = {"name", "height", "birthday", "age"};
	select.where = {{{"", "id", BoolExpr::OP_EQ, "1", ""},}};
	auto result = db->select(select);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({VARCHAR, FLOAT, DATE, INT},
									  {"NewName", "111.1", "1997/04/03", "50"}),
			  result.records[0]);
}

TEST_F(TestUpdate, SetNull)
{
	auto cmd = Update();
	cmd.tableName = "people";
	cmd.sets = {
			{"id_num", ""},
			{"birthday", ""},
			{"height", ""},
			{"age", ""},
	};
	cmd.where = {{{"", "id", BoolExpr::OP_EQ, "1", ""},}};
	db->execute(cmd);

	auto select = Select();
	select.froms = {"people"};
	select.selects = {"id_num", "height", "birthday", "age"};
	select.where = {{{"", "id", BoolExpr::OP_EQ, "1", ""},}};
	auto result = db->select(select);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({CHAR, FLOAT, DATE, INT},
									  {"", "", "", ""}),
			  result.records[0]);
}

TEST_F(TestUpdate, ThrowWhenNameNotExist)
{
	auto cmd = Update();
	cmd.tableName = "people";
	cmd.sets = {{"hehe", "hehe"}};
	ASSERT_THROW( db->execute(cmd), NameNotExistError );

	cmd.tableName = "people";
	cmd.sets = {{"name", "Alice"}};
	cmd.where = {{{"", "hehe", BoolExpr::OP_EQ, "1", ""},}};
	ASSERT_THROW( db->execute(cmd), NameNotExistError );

	cmd.tableName = "people1";
	cmd.sets = {{"name", "Alice"}};
	cmd.where = {};
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

TEST_F(TestUpdate, ThrowWhenNotUnique)
{
	auto cmd = Update();
	cmd.tableName = "people";
	cmd.sets = {{"id", "1"}};
	ASSERT_THROW( db->execute(cmd), NotUniqueError );
}

}