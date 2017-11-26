#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>
#include <stdexcept>

enum class StatementType{
	SHOW_DATABASES,
	CREATE_DATABASE,
	DROP_DATABASE,
	USE_DATABASE,
	SHOW_TABLES
};

struct Statement{
	virtual StatementType getType()=0;
};

struct ShowDatabasesStmt:Statement{
	StatementType getType()override{return StatementType::SHOW_DATABASES;}
};

struct CreateDatabaseStmt:Statement{
	::std::string name;
	StatementType getType()override{return StatementType::CREATE_DATABASE;}
};

struct DropDatabaseStmt:Statement{
	::std::string name;
	StatementType getType()override{return StatementType::DROP_DATABASE;}
};

struct UseDatabaseStmt:Statement{
	::std::string name;
	StatementType getType()override{return StatementType::USE_DATABASE;}
};

struct ShowTablesStmt:Statement{
	StatementType getType()override{return StatementType::SHOW_TABLES;}
};

#endif //STATEMENT_H