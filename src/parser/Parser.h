#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "parser/Tokenizer.h"
#include "ast/TableDef.h"
#include "ast/Command.h"
#include <table/TableRecord.h>

struct Statement;

class Parser{
	Tokenizer tokenizer;
	Token lookahead;
	void nextToken();
	void eatToken(TokenType type,::std::string errorMessage);
	::std::string getIdentifier(::std::string errorMessage);
	::std::vector<::std::unique_ptr<Statement>> parseProgram();
	::std::unique_ptr<Statement> parseStatement();
	::std::unique_ptr<Statement> parseShowStmt();
	::std::unique_ptr<Statement> parseCreateStmt();
	::std::unique_ptr<Statement> parseDescStmt();
	::std::unique_ptr<Statement> parseDropStmt();
	::std::unique_ptr<Statement> parseUseStmt();
	::std::unique_ptr<Statement> parseInsertStmt();
	::std::unique_ptr<Statement> parseDeleteStmt();
	::std::unique_ptr<Statement> parseSelectStmt();
	::std::unique_ptr<Statement> parseUpdateStmt();
	void parseTableDefineField(TableDef& tableDefine,bool& primaryKeySetted);
	void parseTypeDefine(DataType& type,size_t& size);
	void parseColumnConstraint(bool& nullable,bool& unique);
	TableRecord parseTableRecord();
	void parseTableRecordValue(TableRecord& tableRecord);
	void parseWhere(Condition& where);
	BoolExpr parseCondition();
	void parseColumnSpec(::std::string& table,::std::string& column);
	::std::string parseColumnSpecString();
	void parseOp(BoolExpr::Operator& op);
	SetStmt parseUpdateSet();
	Parser(const ::std::string text);
	::std::vector<::std::unique_ptr<Statement>> parse();
public:
	static inline ::std::vector<::std::unique_ptr<Statement>> parseString(const ::std::string text){
		return Parser(text).parse();
	}
};

#endif //PARSER_H