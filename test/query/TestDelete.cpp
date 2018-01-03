#include <ast/Command.h>
#include <query/QueryManager.h>
#include <ast/Exceptions.h>
#include "TestQueryBase.h"

namespace {

class TestDelete : public TestQueryBase
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

TEST_F(TestDelete, Normal)
{
	auto cmd = Delete();
	cmd.tableName = "people";
	cmd.where = {{
		{"", "id", BoolExpr::OP_EQ, "1", ""},
	}};
	db->execute(cmd);
	ASSERT_EQ(2, db->getTable("people")->size());
}

TEST_F(TestDelete, ThrowWhenNameNotExist)
{
	auto cmd = Delete();
	cmd.tableName = "people";
	cmd.where = {{{"", "hehe", BoolExpr::OP_EQ, "1", ""},}};
	ASSERT_THROW( db->execute(cmd), NameNotExistError );

	cmd.tableName = "people1";
	cmd.where = {};
	ASSERT_THROW( db->execute(cmd), NameNotExistError );
}

}