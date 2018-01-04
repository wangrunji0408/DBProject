#include <ast/Command.h>
#include <query/QueryManager.h>
#include <ast/Exceptions.h>
#include "TestQueryBase.h"

namespace {

class TestInsert : public TestQueryBase
{
protected:
	void SetUp() override {
		TestQueryBase::SetUp();
	}

	void Reopen() override {
		TestQueryBase::Reopen();
	}
};

TEST_F(TestInsert, Normal)
{
	insertRecords();
}

TEST_F(TestInsert, ImplicitTypeCast)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
			TableRecord::fromString({INT, CHAR, VARCHAR, UNKNOWN, VARCHAR, INT, INT},
									{"1","Alice","F","","1997/04/02","150","20"}),
	};
	db->execute(cmd);
}

	TEST_F(TestInsert, ThrowWhenDuplicateSinglePrimaryKey)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
		TableRecord::fromString(types, {"1","Alice","F","","","",""}),
		TableRecord::fromString(types, {"1","Bob","M","","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), NotUniqueError );
}

TEST_F(TestInsert, ThrowWhenDuplicateMultiPrimaryKey)
{
	insertRecords();
	auto cmd = Insert();
	cmd.tableName = "borrow";
	cmd.records = {
		TableRecord::fromString({INT,INT}, {"1","1"}),
	};
	ASSERT_THROW( db->execute(cmd), NotUniqueError );
}

TEST_F(TestInsert, ThrowWhenPrimaryKeyIsNull)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
			TableRecord::fromString(types, {"","Alice","F","","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), NullValueError );
}

TEST_F(TestInsert, ThrowWhenForeignKeyNotExist)
{
	insertRecords();
	auto cmd = Insert();
	cmd.tableName = "borrow";
	cmd.records = {
			TableRecord::fromString({INT,INT}, {"1","3"}),
	};
	ASSERT_THROW( db->execute(cmd), ForeignKeyNotExistError );
}

TEST_F(TestInsert, ThrowWhenUniqueError)
{
	insertRecords();
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
		TableRecord::fromString(types, {"4","Dog","F","110104199704021111","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), NotUniqueError );
}

TEST_F(TestInsert, ThrowWhenStringTooLong)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
		TableRecord::fromString(types, {"1","12345678901234567890123456","F","","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), ExecuteError );
}

TEST_F(TestInsert, ThrowWhenFormatError)
{
	ASSERT_ANY_THROW( TableRecord::fromString(types, {"2147483648","Alice","F","","","",""}) );
	ASSERT_ANY_THROW( TableRecord::fromString(types, {"-2147483649","Alice","F","","","",""}) );
	ASSERT_ANY_THROW( TableRecord::fromString(types, {"1.2","Alice","F","","","",""}) );
	ASSERT_ANY_THROW( TableRecord::fromString(types, {"1","Alice","F","","2000/2/30","",""}) );
	vector<TableRecord> cases = {
		TableRecord::fromString({FLOAT, VARCHAR, CHAR, CHAR, DATE, FLOAT, INT},
								{"1.2","Alice","F","","","",""}),
	};
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records.resize(1);
	for(auto const& record: cases) {
		cmd.records[0] = record;
		ASSERT_THROW( db->execute(cmd), ExecuteError );
	}
}

TEST_F(TestInsert, ThrowWhenLengthError)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
		TableRecord::fromString({INT, VARCHAR, CHAR, CHAR, DATE, FLOAT},
								{"1","Alice","F","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), ExecuteError );
}

}