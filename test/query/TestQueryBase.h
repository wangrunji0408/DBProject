#include <ast/Command.h>
#include "../TestBase.h"

class TestQueryBase : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
		createTables();
	}

	void Reopen() override {
		TestBase::Reopen();
	}

	void createTables() {
		auto people = TableDef();
		people.name = "people";
		people.columns = {
			{"id", INT, 4, false, false},
			{"name", VARCHAR, 25, false, false},
			{"gender", CHAR, 1, false, false},
			{"id_num", CHAR, 18, true, true},
			{"birthday", DATE, 4, true, false},
			{"height", FLOAT, 4, true, false}
		};
		people.primaryKeys = {"id"};
		db->createTable(people);
		types = std::vector<DataType>{INT, VARCHAR, CHAR, CHAR, DATE, FLOAT};
	}

	void insertRecords() {
		auto cmd = Insert();
		cmd.tableName = "people";
		cmd.records = {
			TableRecord::fromString(types, {"1","Alice","F","110104199704015833","1997/04/01","160.2"}),
			TableRecord::fromString(types, {"2","Bob","M","110104199704021111","1997/04/02","170.1"}),
			TableRecord::fromString(types, {"3","Cat","M","","",""}),
		};
		db->execute(cmd);
	}

	std::vector<DataType> types;
};