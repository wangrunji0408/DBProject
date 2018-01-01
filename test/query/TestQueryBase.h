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
		types = {INT, VARCHAR, CHAR, CHAR, DATE, FLOAT};

		auto book = TableDef();
		book.name = "book";
		book.columns = {
				{"id", INT, 4, false, false},
				{"name", VARCHAR, 25, false, false},
		};
		book.primaryKeys = {"id"};
		db->createTable(book);

		auto borrow = TableDef();
		borrow.name = "borrow";
		borrow.columns = {
				{"people_id", INT, 4, false, false},
				{"book_id", INT, 4, false, false},
		};
		borrow.primaryKeys = {"people_id", "book_id"};
		borrow.foreignKeys.push_back(ForeignKeyDef{"people_id", "people", "id"});
		borrow.foreignKeys.push_back(ForeignKeyDef{"book_id", "book", "id"});
		db->createTable(borrow);
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

		cmd.tableName = "book";
		cmd.records = {
			TableRecord::fromString({INT, VARCHAR}, {"1","Book1"}),
			TableRecord::fromString({INT, VARCHAR}, {"2","Book2"}),
		};
		db->execute(cmd);

		cmd.tableName = "borrow";
		cmd.records = {
			TableRecord::fromString({INT, INT}, {"1","1"}),
			TableRecord::fromString({INT, INT}, {"1","2"}),
			TableRecord::fromString({INT, INT}, {"2","2"}),
		};
		db->execute(cmd);
	}

	std::vector<DataType> types;
};