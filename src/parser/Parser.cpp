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
	if(lookahead.type==TokenType::K_DROP){
		return parseDropStmt();
	}
	if(lookahead.type==TokenType::K_SHOW){
		return parseShowStmt();
	}
	if(lookahead.type==TokenType::K_USE){
		return parseUseStmt();
	}
	throw std::runtime_error("Expected keyword CREATE/DROP/SHOW/USE");
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
	throw std::runtime_error("Expected keyword DATABASES/SHOW");
}
::std::unique_ptr<Statement> Parser::parseCreateStmt(){
	throw std::runtime_error("Not implemented");
}
::std::unique_ptr<Statement> Parser::parseDropStmt(){
	throw std::runtime_error("Not implemented");
}
::std::unique_ptr<Statement> Parser::parseUseStmt(){
	throw std::runtime_error("Not implemented");
}