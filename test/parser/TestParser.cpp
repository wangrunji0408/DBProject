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
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("create database world;drop index page(homepage)");
	ASSERT_EQ(statements.size(),2);
	ASSERT_EQ(statements[0]->getType(),StatementType::CREATE_DATABASE);
	ASSERT_EQ(statements[1]->getType(),StatementType::CREATE_INDEX);
	ASSERT_EQ(dynamic_cast<CreateDatabaseStmt&>(*statements[0]).database,"world");
	ASSERT_EQ(dynamic_cast<CreateIndexStmt&>(*statements[1]).table,"page");
	ASSERT_EQ(dynamic_cast<CreateIndexStmt&>(*statements[1]).column,"homepage");
}

}