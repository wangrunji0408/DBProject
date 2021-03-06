#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>
#include <stdexcept>
#include "ast/TableDef.h"
#include "ast/Command.h"

enum class StatementType{
	SHOW_DATABASES,
	CREATE_DATABASE,
	DROP_DATABASE,
	USE_DATABASE,
	SHOW_TABLES,
	SHOW_TABLE_SCHEMA,
	CREATE_TABLE,
	DROP_TABLE,
	CREATE_INDEX,
	DROP_INDEX,
	COMMAND
};

struct Statement{
	virtual StatementType getType()=0;
};

struct ShowDatabasesStmt:Statement{
	StatementType getType()override{return StatementType::SHOW_DATABASES;}
};

struct CreateDatabaseStmt:Statement{
	::std::string database;
	StatementType getType()override{return StatementType::CREATE_DATABASE;}
};

struct DropDatabaseStmt:Statement{
	::std::string database;
	StatementType getType()override{return StatementType::DROP_DATABASE;}
};

struct UseDatabaseStmt:Statement{
	::std::string database;
	StatementType getType()override{return StatementType::USE_DATABASE;}
};

struct ShowTablesStmt:Statement{
	StatementType getType()override{return StatementType::SHOW_TABLES;}
};

struct ShowTableSchemaStmt:Statement{
	::std::string table;
	StatementType getType()override{return StatementType::SHOW_TABLE_SCHEMA;}
};

struct CreateTableStmt:Statement{
	TableDef define;
	StatementType getType()override{return StatementType::CREATE_TABLE;}
};

struct DropTableStmt:Statement{
	::std::string table;
	StatementType getType()override{return StatementType::DROP_TABLE;}
};

struct CreateIndexStmt:Statement{
	::std::string table;
	::std::string column;
	StatementType getType()override{return StatementType::CREATE_INDEX;}
};

struct DropIndexStmt:Statement{
	::std::string table;
	::std::string column;
	StatementType getType()override{return StatementType::DROP_INDEX;}
};

struct CommandStmt:Statement{
	::std::unique_ptr<Command> command;
	StatementType getType()override{return StatementType::COMMAND;}
};

#endif //STATEMENT_H