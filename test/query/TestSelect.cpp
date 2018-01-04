#include <ast/Command.h>
#include <query/QueryManager.h>
#include <ast/Exceptions.h>
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
			"1","Alice","F","110104199704015833","1997/04/01","160.2","20"});
	ASSERT_EQ(stdRecord, result.records[0]);
	auto stdColNames = std::vector<std::string> {
			"people.id", "people.name", "people.gender",
			"people.id_num", "people.birthday", "people.height", "people.age"};
	ASSERT_EQ(stdColNames, result.colNames);
}

TEST_F(TestSelect, FromOne_Part)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"people.id", "name"};
	auto result = db->select(cmd);
	ASSERT_EQ(3, result.records.size());
	auto stdRecord = TableRecord::fromString({INT, VARCHAR}, {"1","Alice"});
	ASSERT_EQ(stdRecord, result.records[0]);
	auto stdColNames = std::vector<std::string> {
			"people.id", "people.name"};
	ASSERT_EQ(stdColNames, result.colNames);
}

TEST_F(TestSelect, FromOne_Where_INT)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{{"", "age", BoolExpr::OP_EQ, "20", ""}}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "age", BoolExpr::OP_NE, "20", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[1]);

	cmd.where = {{{"", "age", BoolExpr::OP_GE, "21", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);

	cmd.where = {{{"", "age", BoolExpr::OP_GT, "21", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "age", BoolExpr::OP_LE, "20", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "age", BoolExpr::OP_LT, "20", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "age", BoolExpr::OP_IS_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[0]);

	cmd.where = {{{"", "age", BoolExpr::OP_IS_NOT_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[1]);
}

TEST_F(TestSelect, FromOne_Where_CHAR)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{{"", "id_num", BoolExpr::OP_EQ, "110104199704015833", ""}}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "id_num", BoolExpr::OP_NE, "110104199704015833", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[1]);

	cmd.where = {{{"", "id_num", BoolExpr::OP_GE, "110104199704021111", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);

	cmd.where = {{{"", "id_num", BoolExpr::OP_GT, "110104199704021111", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "id_num", BoolExpr::OP_LE, "110104199704015833", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "id_num", BoolExpr::OP_LT, "110104199704015833", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "id_num", BoolExpr::OP_IS_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[0]);

	cmd.where = {{{"", "id_num", BoolExpr::OP_IS_NOT_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[1]);
}

TEST_F(TestSelect, FromOne_Where_DATE)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{{"", "birthday", BoolExpr::OP_EQ, "1997/04/01", ""}}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "birthday", BoolExpr::OP_NE, "1997/04/01", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[1]);

	cmd.where = {{{"", "birthday", BoolExpr::OP_GE, "1997/04/02", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);

	cmd.where = {{{"", "birthday", BoolExpr::OP_GT, "1997/04/02", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "birthday", BoolExpr::OP_LE, "1997/04/01", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "birthday", BoolExpr::OP_LT, "1997/04/01", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "birthday", BoolExpr::OP_IS_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[0]);

	cmd.where = {{{"", "birthday", BoolExpr::OP_IS_NOT_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[1]);
}

TEST_F(TestSelect, FromOne_Where_FLOAT)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{{"", "height", BoolExpr::OP_EQ, "160.2", ""}}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "height", BoolExpr::OP_NE, "160.2", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[1]);

	cmd.where = {{{"", "height", BoolExpr::OP_GE, "170.1", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);

	cmd.where = {{{"", "height", BoolExpr::OP_GT, "170.1", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "height", BoolExpr::OP_LE, "160.2", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "height", BoolExpr::OP_LT, "160.2", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(0, result.records.size());

	cmd.where = {{{"", "height", BoolExpr::OP_IS_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"3"}), result.records[0]);

	cmd.where = {{{"", "height", BoolExpr::OP_IS_NOT_NULL, "", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[1]);
}

TEST_F(TestSelect, FromOne_Where_OP_LIKE)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{{"", "name", BoolExpr::OP_LIKE, "_lice", ""}}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "name", BoolExpr::OP_LIKE, "%o%", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);

	cmd.where = {{{"", "name", BoolExpr::OP_LIKE, "A[cil]*e", ""}}};
	result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);

	cmd.where = {{{"", "id", BoolExpr::OP_LIKE, "1", ""}}};
	ASSERT_THROW( db->select(cmd), ExecuteError );

	cmd.where = {{{"", "birthday", BoolExpr::OP_LIKE, "1997/04/01", ""}}};
	ASSERT_THROW( db->select(cmd), ExecuteError );

	cmd.where = {{{"", "height", BoolExpr::OP_LIKE, "120.1", ""}}};
	ASSERT_THROW( db->select(cmd), ExecuteError );
}

TEST_F(TestSelect, FromOne_Where_And)
{
	auto cmd = Select();
	cmd.froms = {"people"};
	cmd.selects = {"id"};
	cmd.where = {{
		{"", "id", BoolExpr::OP_LE, "2", ""},
		{"", "name", BoolExpr::OP_GE, "B", ""},
	}};
	auto result = db->select(cmd);
	ASSERT_EQ(1, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[0]);
}

TEST_F(TestSelect, FromTwo)
{
	auto cmd = Select();
	cmd.froms = {"people", "borrow"};
	cmd.selects = {"borrow.book_id"};
	auto result = db->select(cmd);
	ASSERT_EQ(9, result.records.size());
	auto stdColNames = std::vector<std::string> {
			"borrow.book_id"
	};
	ASSERT_EQ(stdColNames, result.colNames);
}

TEST_F(TestSelect, FromTwo_Where)
{
	auto cmd = Select();
	cmd.froms = {"people", "borrow"};
	cmd.selects = {"borrow.book_id"};
	cmd.where = {{
	 	{"people", "id", BoolExpr::OP_EQ, "", "borrow.people_id"},
	 	{"people", "name", BoolExpr::OP_EQ, "Alice", ""}
	}};
	auto result = db->select(cmd);
	ASSERT_EQ(2, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[1]);
}

TEST_F(TestSelect, FromTwo_Where_Unique)
{
	auto cmd = Select();
	cmd.froms = {"people", "borrow"};
	cmd.selects = {"borrow.book_id"};
	cmd.where = {{{"borrow", "people_id", BoolExpr::OP_EQ, "", "people.id"}}};
	auto result = db->select(cmd);
	ASSERT_EQ(3, result.records.size());
	ASSERT_EQ(TableRecord::fromString({INT}, {"1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[1]);
	ASSERT_EQ(TableRecord::fromString({INT}, {"2"}), result.records[2]);
}

TEST_F(TestSelect, FromThree_Where)
{
	auto cmd = Select();
	cmd.froms = {"people", "borrow", "book"};
	cmd.selects = {"people.name", "book.name"};
	cmd.where = {{
		 {"borrow", "people_id", BoolExpr::OP_EQ, "", "people.id"},
		 {"borrow", "book_id", BoolExpr::OP_EQ, "", "book.id"},
	}};
	auto result = db->select(cmd);
	ASSERT_EQ(3, result.records.size());
	ASSERT_EQ(TableRecord::fromString({VARCHAR, VARCHAR}, {"Alice", "Book1"}), result.records[0]);
	ASSERT_EQ(TableRecord::fromString({VARCHAR, VARCHAR}, {"Alice", "Book2"}), result.records[1]);
	ASSERT_EQ(TableRecord::fromString({VARCHAR, VARCHAR}, {"Bob", "Book2"}), result.records[2]);
}

}