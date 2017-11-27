#include "../TestBase.h"
#include "parser/Parser.h"
#include "ast/Statement.h"

namespace {

class TestParser : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
	}

	void Reopen() override {
		TestBase::Reopen();
	}
};

TEST_F(TestParser, HandleEmptyProgram)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString(";;;\n;\n");
	ASSERT_EQ(statements.size(),0);
}

TEST_F(TestParser, HandleShowStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("show databases;show tables;show table pika");
	ASSERT_EQ(statements.size(),3);
	ASSERT_EQ(statements[0]->getType(),StatementType::SHOW_DATABASES);
	ASSERT_EQ(statements[1]->getType(),StatementType::SHOW_TABLES);
	ASSERT_EQ(statements[2]->getType(),StatementType::SHOW_TABLE_SCHEMA);
	ASSERT_EQ(dynamic_cast<ShowTableSchemaStmt&>(*statements[2]).table,"pika");
}

TEST_F(TestParser, HandleUseStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("use neko;use database nyan");
	ASSERT_EQ(statements.size(),2);
	ASSERT_EQ(statements[0]->getType(),StatementType::USE_DATABASE);
	ASSERT_EQ(statements[1]->getType(),StatementType::USE_DATABASE);
	ASSERT_EQ(dynamic_cast<UseDatabaseStmt&>(*statements[0]).database,"neko");
	ASSERT_EQ(dynamic_cast<UseDatabaseStmt&>(*statements[1]).database,"nyan");
}

TEST_F(TestParser, HandleDescStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("desc\nnyanko;;;;");
	ASSERT_EQ(statements.size(),1);
	ASSERT_EQ(statements[0]->getType(),StatementType::SHOW_TABLE_SCHEMA);
	ASSERT_EQ(dynamic_cast<ShowTableSchemaStmt&>(*statements[0]).table,"nyanko");
}

TEST_F(TestParser, HandleDropStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("drop database ditto;drop table mirror;drop index kirby(type)");
	ASSERT_EQ(statements.size(),3);
	ASSERT_EQ(statements[0]->getType(),StatementType::DROP_DATABASE);
	ASSERT_EQ(statements[1]->getType(),StatementType::DROP_TABLE);
	ASSERT_EQ(statements[2]->getType(),StatementType::DROP_INDEX);
	ASSERT_EQ(dynamic_cast<DropDatabaseStmt&>(*statements[0]).database,"ditto");
	ASSERT_EQ(dynamic_cast<DropTableStmt&>(*statements[1]).table,"mirror");
	ASSERT_EQ(dynamic_cast<DropIndexStmt&>(*statements[2]).table,"kirby");
	ASSERT_EQ(dynamic_cast<DropIndexStmt&>(*statements[2]).column,"type");
}

TEST_F(TestParser, HandleSimpleCreateStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("create database world;create index page(homepage)");
	ASSERT_EQ(statements.size(),2);
	ASSERT_EQ(statements[0]->getType(),StatementType::CREATE_DATABASE);
	ASSERT_EQ(statements[1]->getType(),StatementType::CREATE_INDEX);
	ASSERT_EQ(dynamic_cast<CreateDatabaseStmt&>(*statements[0]).database,"world");
	ASSERT_EQ(dynamic_cast<CreateIndexStmt&>(*statements[1]).table,"page");
	ASSERT_EQ(dynamic_cast<CreateIndexStmt&>(*statements[1]).column,"homepage");
}

TEST_F(TestParser, HandleCreateTableStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("create table pika(chu int(10),nyan float not null,err date,pie varchar(1000) not null,primary key(chu,nyan),foreign key(err)references world(end),foreign key(pie)references eat(able))");
	ASSERT_EQ(statements.size(),1);
	ASSERT_EQ(statements[0]->getType(),StatementType::CREATE_TABLE);
	auto define = TableDef();
	define.name="pika";
	define.columns.push_back(ColumnDef{"chu",INT,10,true,false});
	define.columns.push_back(ColumnDef{"nyan",FLOAT,4,false,false});
	define.columns.push_back(ColumnDef{"err",DATE,4,true,false});
	define.columns.push_back(ColumnDef{"pie",VARCHAR,1000,false,false});
	define.primaryKeys.emplace_back("chu");
	define.primaryKeys.emplace_back("nyan");
	define.foreignKeys.push_back(ForeignKeyDef{"err","world","end"});
	define.foreignKeys.push_back(ForeignKeyDef{"pie","eat","able"});
	ASSERT_EQ(define,dynamic_cast<CreateTableStmt&>(*statements[0]).define);
}

}