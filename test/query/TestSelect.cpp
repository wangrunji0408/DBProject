#include <ast/Command.h>
#include <query/QueryManager.h>
#include "TestQueryBase.h"

namespace {

class TestSelect : public TestQueryBase
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

TEST_F(TestSelect, FromOneTable)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.where = {{
		{"", "id", BoolExpr::OP_EQ, "1", ""},
	}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	auto expected = TableRecord::fromString(types, {"1","Alice","F","110104199704015833","1997/04/01","160.2"});
	ASSERT_EQ(expected, result.records[0]);
}

}