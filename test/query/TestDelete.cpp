#include <ast/Command.h>
#include <query/QueryManager.h>
#include "../TestBase.h"

namespace {

class TestDelete : public TestBase
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
		customer.columns = {
			{"id", INT, 4, false, false},
			{"name", VARCHAR, 25, false, false},
			{"gender", VARCHAR, 1, false, false},
		};
		customer.primaryKeys = {"id"};
		db->createTable(customer);

		auto cmd = Insert();
		cmd.tableName = "customer";
		cmd.records = {
			{{"300001","CHAD CABELLO","F"}},
			{{"300002","FAUSTO VANNORMAN","F"}},
		};
		db->execute(cmd);
	}

	void Reopen() override {
		TestBase::Reopen();
	}
};

TEST_F(TestDelete, Normal)
{
	auto cmd = Delete();
	cmd.tableName = "customer";
	cmd.where = {{
		{"", "id", BoolExpr::OP_EQ, "300001", ""},
	}};
	db->execute(cmd);
	ASSERT_EQ(1, db->getTable("customer")->size());
}

}