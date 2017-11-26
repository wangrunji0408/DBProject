#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "parser/Tokenizer.h"

struct Statement;

class Parser{
	Tokenizer tokenizer;
	Token lookahead;
	void nextToken();
	::std::vector<::std::unique_ptr<Statement>> parseProgram();
	::std::unique_ptr<Statement> parseStatement();
	::std::unique_ptr<Statement> parseShowStmt();
	::std::unique_ptr<Statement> parseCreateStmt();
	::std::unique_ptr<Statement> parseDropStmt();
	::std::unique_ptr<Statement> parseUseStmt();
	Parser(const ::std::string text);
	::std::vector<::std::unique_ptr<Statement>> parse();
public:
	static inline ::std::vector<::std::unique_ptr<Statement>> parseString(const ::std::string text){
		return Parser(text).parse();
	}
};

#endif //PARSER_H