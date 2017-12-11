#include <ast/Command.h>
#include <query/QueryManager.h>
#include "../TestBase.h"

namespace {

using namespace CommandDef;

class TestInsert : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		/*
			CREATE TABLE customer(
				id INT(10) NOT NULL,
				name VARCHAR(25) NOT NULL,
				gender VARCHAR(1) NOT NULL,
				PRIMARY KEY (id)
			);
		*/
		auto customer = TableDef();
		customer.name = "customer";
		customer.columns.push_back(ColumnDef{"id", INT, 4, false, false});
		customer.columns.push_back(ColumnDef{"name", VARCHAR, 25, false, false});
		customer.columns.push_back(ColumnDef{"gender", VARCHAR, 1, false, false});
		customer.primaryKeys.emplace_back("id");
		db->createTable(customer);

		qm = new QueryManager(*db);
	}

	void Reopen() override {
		TestBase::Reopen();
	}

	QueryManager* qm = nullptr;
};

TEST_F(TestInsert, Normal)
{
	auto cmd = Insert();
	cmd.tableName = "customer";
	cmd.records.push_back(RecordValue{{"300001","CHAD CABELLO","F"}});
	cmd.records.push_back(RecordValue{{"300002","FAUSTO VANNORMAN","F"}});

	qm->execute(cmd);
}

TEST_F(TestInsert, ThrowWhenDuplicatePrimaryKey)
{
	auto cmd = Insert();
	cmd.tableName = "customer";
	cmd.records.push_back(RecordValue{{"300001","CHAD CABELLO","F"}});
	cmd.records.push_back(RecordValue{{"300001","FAUSTO VANNORMAN","F"}});

	ASSERT_ANY_THROW( qm->execute(cmd) );
}

TEST_F(TestInsert, ThrowWhenStringTooLong)
{
	auto cmd = Insert();
	cmd.tableName = "customer";
	cmd.records.push_back(RecordValue{{"300001","01234567890123456789012345","F"}});
	ASSERT_ANY_THROW( qm->execute(cmd) );
}

TEST_F(TestInsert, ThrowWhenFormatError)
{
	vector<RecordValue> cases = {
			{{"2147483648","CHAD CABELLO","F"}},
			{{"-2147483649","CHAD CABELLO","F"}},
			{{"1.2","CHAD CABELLO","F"}},
	};
	auto cmd = Insert();
	cmd.tableName = "customer";
	cmd.records.resize(1);
	for(auto const& record: cases) {
		cmd.records[0] = record;
		ASSERT_ANY_THROW( qm->execute(cmd) );
	}
}

}