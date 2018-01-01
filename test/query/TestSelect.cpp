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

TEST_F(TestSelect, FromOne_All)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"*"};
	auto result = db->select(cmd);
	ASSERT_EQ(3, result.records.size());
	auto stdRecord = TableRecord::fromString(types, {
			"1","Alice","F","110104199704015833","1997/04/01","160.2"});
	ASSERT_EQ(stdRecord, result.records[0]);
	auto stdColNames = std::vector<std::string> {
			"people.id", "people.name", "people.gender",
			"people.id_num", "people.birthday", "people.height"};
	ASSERT_EQ(stdColNames, result.colNames);
}

TEST_F(TestSelect, FromOne_Part)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id", "name"};
	auto result = db->select(cmd);
	ASSERT_EQ(3, result.records.size());
	auto stdRecord = TableRecord::fromString({INT, VARCHAR}, {"1","Alice"});
	ASSERT_EQ(stdRecord, result.records[0]);
	auto stdColNames = std::vector<std::string> {
			"people.id", "people.name"};
	ASSERT_EQ(stdColNames, result.colNames);
}

TEST_F(TestSelect, FromOne_Where)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{
		 {"", "id", BoolExpr::OP_EQ, "1", ""},
	}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	auto stdRecord = TableRecord::fromString({INT}, {"1"});
	ASSERT_EQ(stdRecord, result.records[0]);
}

}