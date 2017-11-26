#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "parser/Tokenizer.h"
#include "ast/TableDef.h"

struct Statement;

class Parser{
	Tokenizer tokenizer;
	Token lookahead;
	void nextToken();
	void eatToken(TokenType type,const char* errorMessage);
	::std::string getIdentifier(const char* errorMessage);
	::std::vector<::std::unique_ptr<Statement>> parseProgram();
	::std::unique_ptr<Statement> parseStatement();
	::std::unique_ptr<Statement> parseShowStmt();
	::std::unique_ptr<Statement> parseCreateStmt();
	::std::unique_ptr<Statement> parseDescStmt();
	::std::unique_ptr<Statement> parseDropStmt();
	::std::unique_ptr<Statement> parseUseStmt();
	void parseTableDefineField(TableDef& tableDefine,bool& primaryKeySetted);
	Parser(const ::std::string text);
	::std::vector<::std::unique_ptr<Statement>> parse();
public:
	static inline ::std::vector<::std::unique_ptr<Statement>> parseString(const ::std::string text){
		return Parser(text).parse();
	}
};

#endif //PARSER_H