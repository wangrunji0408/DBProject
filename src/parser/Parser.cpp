#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "parser/Parser.h"
#include "parser/Tokenizer.h"
#include "ast/Statement.h"

Parser::Parser(const ::std::string text):tokenizer(text){
	nextToken();
}

void Parser::nextToken(){
	lookahead=tokenizer.next();
}

void Parser::eatToken(TokenType type,const char* errorMessage){
	if(lookahead.type!=type){
		throw std::runtime_error(errorMessage);
	}
	nextToken();
}


::std::string Parser::getIdentifier(const char* errorMessage){
	if(lookahead.type!=TokenType::IDENTIFIER){
		throw std::runtime_error(errorMessage);
	}
	::std::string name=lookahead.stringValue;
	nextToken();
	return name;
}

::std::vector<::std::unique_ptr<Statement>> Parser::parse(){
	::std::vector<::std::unique_ptr<Statement>> statements=parseProgram();
	if(lookahead.type!=TokenType::EOS){
		throw std::runtime_error("Unexpected trailing token");
	}
	return statements;
}

::std::vector<::std::unique_ptr<Statement>> Parser::parseProgram(){
	::std::vector<::std::unique_ptr<Statement>> statements;
	while(lookahead.type==TokenType::P_SEMICOLON){
		nextToken();
	}
	while(lookahead.type!=TokenType::EOS){
		statements.push_back(parseStatement());
		if(lookahead.type==TokenType::EOS){
			break;
		}
		if(lookahead.type!=TokenType::P_SEMICOLON){
			throw std::runtime_error("Missing semicolon");
		}
		while(lookahead.type==TokenType::P_SEMICOLON){
			nextToken();
		}
	}
	return statements;
}
::std::unique_ptr<Statement> Parser::parseStatement(){
	if(lookahead.type==TokenType::K_CREATE){
		return parseCreateStmt();
	}
	if(lookahead.type==TokenType::K_DESC){
		return parseDescStmt();
	}
	if(lookahead.type==TokenType::K_DROP){
		return parseDropStmt();
	}
	if(lookahead.type==TokenType::K_SHOW){
		return parseShowStmt();
	}
	if(lookahead.type==TokenType::K_USE){
		return parseUseStmt();
	}
	throw std::runtime_error("Expected keyword CREATE/DESC/DROP/SHOW/USE");
}
::std::unique_ptr<Statement> Parser::parseShowStmt(){
	eatToken(TokenType::K_SHOW,"Expected keyword SHOW");
	if(lookahead.type==TokenType::K_DATABASES){
		nextToken();
		return ::std::make_unique<ShowDatabasesStmt>();
	}
	if(lookahead.type==TokenType::K_TABLES){
		nextToken();
		return ::std::make_unique<ShowTablesStmt>();
	}
	if(lookahead.type==TokenType::K_TABLE){
		nextToken();
		::std::unique_ptr<ShowTableSchemaStmt> showStmt=::std::make_unique<ShowTableSchemaStmt>();
		showStmt->table=getIdentifier("Expected table name");
		return ::std::move(showStmt);
	}
	throw std::runtime_error("Expected keyword DATABASES/TABLES/TABLE");
}
::std::unique_ptr<Statement> Parser::parseCreateStmt(){
	eatToken(TokenType::K_CREATE,"Expected keyword CREATE");
	if(lookahead.type==TokenType::K_DATABASES){
		nextToken();
		::std::unique_ptr<CreateDatabaseStmt> createStmt=::std::make_unique<CreateDatabaseStmt>();
		createStmt->database=getIdentifier("Expected database name");
		return ::std::move(createStmt);
	}
	if(lookahead.type==TokenType::K_INDEX){
		nextToken();
		::std::unique_ptr<CreateIndexStmt> createStmt=::std::make_unique<CreateIndexStmt>();
		createStmt->table=getIdentifier("Expected table name");
		eatToken(TokenType::P_LPARENT,"Expected '('");
		createStmt->column=getIdentifier("Expected column name");
		eatToken(TokenType::P_RPARENT,"Expected ')'");
		return ::std::move(createStmt);
	}
	if(lookahead.type==TokenType::K_TABLE){
		nextToken();
		::std::unique_ptr<CreateTableStmt> createStmt=::std::make_unique<CreateTableStmt>();
		bool primaryKeySetted=false;
		createStmt->table=getIdentifier("Expected table name");
		eatToken(TokenType::P_LPARENT,"Expected '('");
		parseTableDefineField(createStmt->define,primaryKeySetted);
		while(lookahead.type==TokenType::P_COMMA){
			nextToken();
			parseTableDefineField(createStmt->define,primaryKeySetted);
		}
		eatToken(TokenType::P_RPARENT,"Expected ')'");
		return ::std::move(createStmt);
	}
	throw std::runtime_error("Expected keyword DATABASE/TABLE/INDEX");
}

::std::unique_ptr<Statement> Parser::parseDescStmt(){
	eatToken(TokenType::K_DESC,"Expected keyword DESC");
	::std::unique_ptr<ShowTableSchemaStmt> showStmt=::std::make_unique<ShowTableSchemaStmt>();
	showStmt->table=getIdentifier("Expected table name");
	return ::std::move(showStmt);
}

::std::unique_ptr<Statement> Parser::parseDropStmt(){
	eatToken(TokenType::K_DROP,"Expected keyword DROP");
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
		::std::unique_ptr<DropDatabaseStmt> dropStmt=::std::make_unique<DropDatabaseStmt>();
		dropStmt->database=getIdentifier("Expected database name");
		return ::std::move(dropStmt);
	}
	if(lookahead.type==TokenType::K_TABLE){
		nextToken();
		::std::unique_ptr<DropTableStmt> dropStmt=::std::make_unique<DropTableStmt>();
		dropStmt->table=getIdentifier("Expected table name");
		return ::std::move(dropStmt);
	}
	if(lookahead.type==TokenType::K_INDEX){
		nextToken();
		::std::unique_ptr<DropIndexStmt> dropStmt=::std::make_unique<DropIndexStmt>();
		dropStmt->table=getIdentifier("Expected table name");
		eatToken(TokenType::P_LPARENT,"Expected '('");
		dropStmt->column=getIdentifier("Expected column name");
		eatToken(TokenType::P_RPARENT,"Expected ')'");
		return ::std::move(dropStmt);
	}
	throw std::runtime_error("Expected keyword DATABASE/TABLE/INDEX");
}
::std::unique_ptr<Statement> Parser::parseUseStmt(){
	eatToken(TokenType::K_USE,"Expected keyword USE");
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
	}
	::std::unique_ptr<UseDatabaseStmt> useStmt=::std::make_unique<UseDatabaseStmt>();
	useStmt->database=getIdentifier("Expected database name");
	return ::std::move(useStmt);
}
void Parser::parseTableDefineField(TableDef& tableDefine,bool& primaryKeySetted){
	if(lookahead.type==TokenType::IDENTIFIER){
		nextToken();
		throw std::runtime_error("Not implemented");
	}
	if(lookahead.type==TokenType::K_PRIMARY){
		if(primaryKeySetted){
			throw std::runtime_error("Duplicated primary key declaration");
		}
		primaryKeySetted=true;
		nextToken();
		throw std::runtime_error("Not implemented");
	}
	if(lookahead.type==TokenType::K_FOREIGN){
		nextToken();
		throw std::runtime_error("Not implemented");
	}
	throw std::runtime_error("Expected column name or keyword PRIMARY/FOREIGN");
}