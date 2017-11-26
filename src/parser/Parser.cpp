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
	if(lookahead.type!=TokenType::K_SHOW){
		throw std::runtime_error("Expected keyword SHOW");
	}
	nextToken();
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
		if(lookahead.type!=TokenType::IDENTIFIER){
			throw std::runtime_error("Expected table name");
		}
		::std::unique_ptr<ShowTableSchemaStmt> showStmt=::std::make_unique<ShowTableSchemaStmt>();
		showStmt->table=lookahead.stringValue;
		nextToken();
		return ::std::move(showStmt);
	}
	throw std::runtime_error("Expected keyword DATABASES/TABLES/TABLE");
}
::std::unique_ptr<Statement> Parser::parseCreateStmt(){
	if(lookahead.type!=TokenType::K_CREATE){
		throw std::runtime_error("Expected keyword CREATE");
	}
	nextToken();
	throw std::runtime_error("Not implemented");
}

::std::unique_ptr<Statement> Parser::parseDescStmt(){
	if(lookahead.type!=TokenType::K_DESC){
		throw std::runtime_error("Expected keyword DESC");
	}
	nextToken();
	if(lookahead.type!=TokenType::IDENTIFIER){
		throw std::runtime_error("Expected table name");
	}
	::std::unique_ptr<ShowTableSchemaStmt> showStmt=::std::make_unique<ShowTableSchemaStmt>();
	showStmt->table=lookahead.stringValue;
	nextToken();
	return ::std::move(showStmt);
}

::std::unique_ptr<Statement> Parser::parseDropStmt(){
	if(lookahead.type!=TokenType::K_DROP){
		throw std::runtime_error("Expected keyword DROP");
	}
	nextToken();
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
		if(lookahead.type!=TokenType::IDENTIFIER){
			throw std::runtime_error("Expected database name");
		}
		::std::unique_ptr<DropDatabaseStmt> dropStmt=::std::make_unique<DropDatabaseStmt>();
		dropStmt->database=lookahead.stringValue;
		nextToken();
		return ::std::move(dropStmt);
	}
	if(lookahead.type==TokenType::K_TABLE){
		nextToken();
		if(lookahead.type!=TokenType::IDENTIFIER){
			throw std::runtime_error("Expected table name");
		}
		::std::unique_ptr<DropTableStmt> dropStmt=::std::make_unique<DropTableStmt>();
		dropStmt->table=lookahead.stringValue;
		nextToken();
		return ::std::move(dropStmt);
	}
	if(lookahead.type==TokenType::K_INDEX){
		nextToken();
		if(lookahead.type!=TokenType::IDENTIFIER){
			throw std::runtime_error("Expected table name");
		}
		::std::unique_ptr<DropIndexStmt> dropStmt=::std::make_unique<DropIndexStmt>();
		dropStmt->table=lookahead.stringValue;
		nextToken();
		if(lookahead.type!=TokenType::P_LPARENT){
			throw std::runtime_error("Expected '('");
		}
		nextToken();
		if(lookahead.type!=TokenType::IDENTIFIER){
			throw std::runtime_error("Expected column name");
		}
		dropStmt->column=lookahead.stringValue;
		nextToken();
		if(lookahead.type!=TokenType::P_RPARENT){
			throw std::runtime_error("Expected ')'");
		}
		nextToken();
		return ::std::move(dropStmt);
	}
	throw std::runtime_error("Expected keyword DATABASE/TABLE/INDEX");
}
::std::unique_ptr<Statement> Parser::parseUseStmt(){
	if(lookahead.type!=TokenType::K_USE){
		throw std::runtime_error("Expected keyword USE");
	}
	nextToken();
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
	}
	if(lookahead.type!=TokenType::IDENTIFIER){
		throw std::runtime_error("Expected database name");
	}
	::std::unique_ptr<UseDatabaseStmt> useStmt=::std::make_unique<UseDatabaseStmt>();
	useStmt->database=lookahead.stringValue;
	nextToken();
	return ::std::move(useStmt);
}