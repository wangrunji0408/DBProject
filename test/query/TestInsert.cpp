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

TEST_F(TestInsert, ThrowWhenDuplicatePrimaryKey)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
		TableRecord::fromString(types, {"1","Alice","F","","",""}),
		TableRecord::fromString(types, {"1","Bob","M","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), ExecuteError );
}

TEST_F(TestInsert, ThrowWhenStringTooLong)
{
	auto cmd = Insert();
	cmd.tableName = "people";
	cmd.records = {
		TableRecord::fromString(types, {"1","12345678901234567890123456","F","","",""}),
	};
	ASSERT_THROW( db->execute(cmd), ExecuteError );
}

TEST_F(TestInsert, ThrowWhenFormatError)
{
	vector<TableRecord> cases = {
		TableRecord::fromString(types, {"2147483648","Alice","F","","",""}),
		TableRecord::fromString(types, {"-2147483649","Alice","F","","",""}),
		TableRecord::fromString(types, {"1.2","Alice","F","","",""}),
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
		TableRecord::fromString(types, {"1","Alice","F","",""}),
	};
	ASSERT_THROW( db->execute(cmd), ExecuteError );
}

}