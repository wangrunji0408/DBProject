#include <ast/Command.h>
#include <query/QueryManager.h>
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
		{"", "id", BoolExpr::OP_EQ, "300001", ""},
	}};
	db->execute(cmd);
	ASSERT_EQ(1, db->getTable("people")->size());
}

}