#include "../TestBase.h"
#include "parser/Parser.h"
#include "ast/Statement.h"
#include "ast/Command.h"

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
	auto statements=Parser::parseString(";;;\n;\n");
	ASSERT_EQ(statements.size(),0);
}

TEST_F(TestParser, HandleShowStatements)
{
	auto statements=Parser::parseString("show databases;show tables;show table pika");
	ASSERT_EQ(statements.size(),3);
	ASSERT_EQ(statements[0]->getType(),StatementType::SHOW_DATABASES);
	ASSERT_EQ(statements[1]->getType(),StatementType::SHOW_TABLES);
	ASSERT_EQ(statements[2]->getType(),StatementType::SHOW_TABLE_SCHEMA);
	ASSERT_EQ(dynamic_cast<ShowTableSchemaStmt&>(*statements[2]).table,"pika");
}

TEST_F(TestParser, HandleUseStatements)
{
	auto statements=Parser::parseString("use neko;use database nyan");
	ASSERT_EQ(statements.size(),2);
	ASSERT_EQ(statements[0]->getType(),StatementType::USE_DATABASE);
	ASSERT_EQ(statements[1]->getType(),StatementType::USE_DATABASE);
	ASSERT_EQ(dynamic_cast<UseDatabaseStmt&>(*statements[0]).database,"neko");
	ASSERT_EQ(dynamic_cast<UseDatabaseStmt&>(*statements[1]).database,"nyan");
}

TEST_F(TestParser, HandleDescStatements)
{
	auto statements=Parser::parseString("desc\nnyanko;;;;");
	ASSERT_EQ(statements.size(),1);
	ASSERT_EQ(statements[0]->getType(),StatementType::SHOW_TABLE_SCHEMA);
	ASSERT_EQ(dynamic_cast<ShowTableSchemaStmt&>(*statements[0]).table,"nyanko");
}

TEST_F(TestParser, HandleDropStatements)
{
	auto statements=Parser::parseString("drop database ditto;drop table mirror;drop index kirby(type)");
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
	auto statements=Parser::parseString("create database world;create index page(homepage)");
	ASSERT_EQ(statements.size(),2);
	ASSERT_EQ(statements[0]->getType(),StatementType::CREATE_DATABASE);
	ASSERT_EQ(statements[1]->getType(),StatementType::CREATE_INDEX);
	ASSERT_EQ(dynamic_cast<CreateDatabaseStmt&>(*statements[0]).database,"world");
	ASSERT_EQ(dynamic_cast<CreateIndexStmt&>(*statements[1]).table,"page");
	ASSERT_EQ(dynamic_cast<CreateIndexStmt&>(*statements[1]).column,"homepage");
}

TEST_F(TestParser, HandleCreateTableStatements)
{
	auto statements=Parser::parseString("create table pika(chu int(10),nyan float not null unique,err date unique,pie varchar(1000) not null,primary key(chu,nyan),foreign key(err)references world(end),foreign key(pie)references eat(able))");
	ASSERT_EQ(statements.size(),1);
	ASSERT_EQ(statements[0]->getType(),StatementType::CREATE_TABLE);
	auto define = TableDef();
	define.name="pika";
	define.columns.push_back(ColumnDef{"chu",INT,10,true,false});
	define.columns.push_back(ColumnDef{"nyan",FLOAT,4,false,true});
	define.columns.push_back(ColumnDef{"err",DATE,4,true,true});
	define.columns.push_back(ColumnDef{"pie",VARCHAR,1000,false,false});
	define.primaryKeys.emplace_back("chu");
	define.primaryKeys.emplace_back("nyan");
	define.foreignKeys.push_back(ForeignKeyDef{"err","world","end"});
	define.foreignKeys.push_back(ForeignKeyDef{"pie","eat","able"});
	ASSERT_EQ(define,dynamic_cast<CreateTableStmt&>(*statements[0]).define);
}

TEST_F(TestParser, HandleInsertCommandStatements)
{
	auto statements=Parser::parseString("insert into world values ('2099-12-31',100,null,'null'),(5.5)");
	ASSERT_EQ(statements.size(),1);
	ASSERT_EQ(statements[0]->getType(),StatementType::COMMAND);
	auto& command=dynamic_cast<CommandStmt&>(*statements[0]).command;
	ASSERT_EQ(command->getType(),CMD_INSERT);
	ASSERT_EQ(dynamic_cast<Insert&>(*command).tableName,"world");
	ASSERT_EQ(dynamic_cast<Insert&>(*command).records.size(),2);
	TableRecord tr0;
	tr0.pushVarchar("2099-12-31").pushInt(100).pushNull(UNKNOWN).pushVarchar("null");
	ASSERT_EQ(dynamic_cast<Insert&>(*command).records[0],tr0);
	ASSERT_EQ(dynamic_cast<Insert&>(*command).records[1].size(),1);
	ASSERT_EQ(dynamic_cast<Insert&>(*command).records[1].getTypeAtCol(0),FLOAT);
	ASSERT_DOUBLE_EQ(*(float*)(dynamic_cast<Insert&>(*command).records[1].getDataAtCol(0).data()),5.5);
}

TEST_F(TestParser, HandleDeleteCommandStatements)
{
	auto statements=Parser::parseString("delete from sekai where human is not null and love is null and you<>me and hate>100");
	ASSERT_EQ(statements.size(),1);
	ASSERT_EQ(statements[0]->getType(),StatementType::COMMAND);
	auto& command=dynamic_cast<CommandStmt&>(*statements[0]).command;
	ASSERT_EQ(command->getType(),CMD_DELETE);
	ASSERT_EQ(dynamic_cast<Delete&>(*command).tableName,"sekai");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands.size(),4);
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[0].tableName,"");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[0].columnName,"human");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[0].op,BoolExpr::OP_IS_NOT_NULL);
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[1].tableName,"");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[1].columnName,"love");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[1].op,BoolExpr::OP_IS_NULL);
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[2].tableName,"");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[2].columnName,"you");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[2].op,BoolExpr::OP_NE);
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[2].rhsAttr,"me");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[3].tableName,"");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[3].columnName,"hate");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[3].op,BoolExpr::OP_GT);
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[3].rhsAttr,"");
	ASSERT_EQ(dynamic_cast<Delete&>(*command).where.ands[3].rhsValue,"100");

}

}