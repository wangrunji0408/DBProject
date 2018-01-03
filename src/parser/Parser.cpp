#include <string>
#include <vector>
#include <iostream>
#include "parser/Parser.h"
#include "parser/Tokenizer.h"
#include "ast/Statement.h"
#include "ast/Command.h"
#include "ast/Exceptions.h"
#include "table/TableRecord.h"
using namespace std::literals::string_literals;

Parser::Parser(const ::std::string text):tokenizer(text){
	nextToken();
}

void Parser::nextToken(){
	lookahead=tokenizer.next();
}

void Parser::eatToken(TokenType type,::std::string errorMessage){
	if(lookahead.type!=type){
		throw ParseError(errorMessage);
	}
	nextToken();
}


::std::string Parser::getIdentifier(::std::string errorMessage){
	if(lookahead.type!=TokenType::IDENTIFIER){
		throw ParseError(errorMessage);
	}
	::std::string name=lookahead.stringValue;
	nextToken();
	return name;
}

::std::vector<::std::unique_ptr<Statement>> Parser::parse(){
	::std::vector<::std::unique_ptr<Statement>> statements=parseProgram();
	if(lookahead.type!=TokenType::EOS){
		throw ParseError("Unexpected trailing token"s);
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
			throw ParseError("Missing semicolon"s);
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
	if(lookahead.type==TokenType::K_DELETE){
		return parseDeleteStmt();
	}
	if(lookahead.type==TokenType::K_DESC){
		return parseDescStmt();
	}
	if(lookahead.type==TokenType::K_DROP){
		return parseDropStmt();
	}
	if(lookahead.type==TokenType::K_INSERT){
		return parseInsertStmt();
	}
	if(lookahead.type==TokenType::K_SELECT){
		return parseSelectStmt();
	}
	if(lookahead.type==TokenType::K_SHOW){
		return parseShowStmt();
	}
	if(lookahead.type==TokenType::K_USE){
		return parseUseStmt();
	}
	throw ParseError("Expected keyword CREATE/DELETE/DESC/DROP/INSERT/SELECT/SHOW/USE"s);
}
::std::unique_ptr<Statement> Parser::parseShowStmt(){
	eatToken(TokenType::K_SHOW,"Expected keyword SHOW"s);
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
		showStmt->table=getIdentifier("Expected table name"s);
		return ::std::move(showStmt);
	}
	throw ParseError("Expected keyword DATABASES/TABLES/TABLE"s);
}
::std::unique_ptr<Statement> Parser::parseCreateStmt(){
	eatToken(TokenType::K_CREATE,"Expected keyword CREATE"s);
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
		::std::unique_ptr<CreateDatabaseStmt> createStmt=::std::make_unique<CreateDatabaseStmt>();
		createStmt->database=getIdentifier("Expected database name"s);
		return ::std::move(createStmt);
	}
	if(lookahead.type==TokenType::K_INDEX){
		nextToken();
		::std::unique_ptr<CreateIndexStmt> createStmt=::std::make_unique<CreateIndexStmt>();
		createStmt->table=getIdentifier("Expected table name"s);
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		createStmt->column=getIdentifier("Expected column name"s);
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		return ::std::move(createStmt);
	}
	if(lookahead.type==TokenType::K_TABLE){
		nextToken();
		::std::unique_ptr<CreateTableStmt> createStmt=::std::make_unique<CreateTableStmt>();
		bool primaryKeySetted=false;
		createStmt->define.name=getIdentifier("Expected table name"s);
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		parseTableDefineField(createStmt->define,primaryKeySetted);
		while(lookahead.type==TokenType::P_COMMA){
			nextToken();
			parseTableDefineField(createStmt->define,primaryKeySetted);
		}
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		return ::std::move(createStmt);
	}
	throw ParseError("Expected keyword DATABASE/TABLE/INDEX"s);
}

::std::unique_ptr<Statement> Parser::parseDescStmt(){
	eatToken(TokenType::K_DESC,"Expected keyword DESC"s);
	::std::unique_ptr<ShowTableSchemaStmt> showStmt=::std::make_unique<ShowTableSchemaStmt>();
	showStmt->table=getIdentifier("Expected table name"s);
	return ::std::move(showStmt);
}

::std::unique_ptr<Statement> Parser::parseDropStmt(){
	eatToken(TokenType::K_DROP,"Expected keyword DROP"s);
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
		::std::unique_ptr<DropDatabaseStmt> dropStmt=::std::make_unique<DropDatabaseStmt>();
		dropStmt->database=getIdentifier("Expected database name"s);
		return ::std::move(dropStmt);
	}
	if(lookahead.type==TokenType::K_TABLE){
		nextToken();
		::std::unique_ptr<DropTableStmt> dropStmt=::std::make_unique<DropTableStmt>();
		dropStmt->table=getIdentifier("Expected table name"s);
		return ::std::move(dropStmt);
	}
	if(lookahead.type==TokenType::K_INDEX){
		nextToken();
		::std::unique_ptr<DropIndexStmt> dropStmt=::std::make_unique<DropIndexStmt>();
		dropStmt->table=getIdentifier("Expected table name"s);
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		dropStmt->column=getIdentifier("Expected column name"s);
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		return ::std::move(dropStmt);
	}
	throw ParseError("Expected keyword DATABASE/TABLE/INDEX"s);
}
::std::unique_ptr<Statement> Parser::parseUseStmt(){
	eatToken(TokenType::K_USE,"Expected keyword USE"s);
	if(lookahead.type==TokenType::K_DATABASE){
		nextToken();
	}
	::std::unique_ptr<UseDatabaseStmt> useStmt=::std::make_unique<UseDatabaseStmt>();
	useStmt->database=getIdentifier("Expected database name"s);
	return ::std::move(useStmt);
}
::std::unique_ptr<Statement> Parser::parseInsertStmt(){
	eatToken(TokenType::K_INSERT,"Expected keyword INSERT"s);
	eatToken(TokenType::K_INTO,"Expected keyword INTO"s);
	::std::unique_ptr<Insert> insertCmd=::std::make_unique<Insert>();
	insertCmd->tableName=getIdentifier("Expected table name"s);
	eatToken(TokenType::K_VALUES,"Expected keyword VALUES"s);
	insertCmd->records.push_back(parseTableRecord());
	while(lookahead.type==TokenType::P_COMMA){
		nextToken();
		insertCmd->records.push_back(parseTableRecord());
	}
	::std::unique_ptr<CommandStmt> cmdStmt=::std::make_unique<CommandStmt>();
	cmdStmt->command=::std::move(insertCmd);
	return ::std::move(cmdStmt);
}
::std::unique_ptr<Statement> Parser::parseDeleteStmt(){
	eatToken(TokenType::K_DELETE,"Expected keyword DELETE"s);
	eatToken(TokenType::K_FROM,"Expected keyword FROM"s);
	::std::unique_ptr<Delete> deleteCmd=::std::make_unique<Delete>();
	deleteCmd->tableName=getIdentifier("Expected table name"s);
	if(lookahead.type==TokenType::K_WHERE){
		nextToken();
		parseWhere(deleteCmd->where);
	}
	::std::unique_ptr<CommandStmt> cmdStmt=::std::make_unique<CommandStmt>();
	cmdStmt->command=::std::move(deleteCmd);
	return ::std::move(cmdStmt);
}
::std::unique_ptr<Statement> Parser::parseSelectStmt(){
	eatToken(TokenType::K_SELECT,"Expected keyword SELECT"s);
	::std::unique_ptr<Select> selectCmd=::std::make_unique<Select>();
	if(lookahead.type==TokenType::P_STAR){
		nextToken();
		selectCmd->selects.push_back("*");
	}else{
		if(lookahead.type!=TokenType::IDENTIFIER){
			throw ParseError("Expected column specification or '*'"s);
		}
		selectCmd->selects.push_back(parseColumnSpecString());
		while(lookahead.type==TokenType::P_COMMA){
			nextToken();
			selectCmd->selects.push_back(parseColumnSpecString());
		}
	}
	eatToken(TokenType::K_FROM,"Expected keyword FROM"s);
	selectCmd->froms.push_back(getIdentifier("Expected table name"s));
	while(lookahead.type==TokenType::P_COMMA){
		nextToken();
		selectCmd->froms.push_back(getIdentifier("Expected table name"s));
	}
	if(lookahead.type==TokenType::K_WHERE){
		nextToken();
		parseWhere(selectCmd->where);
	}
	if(lookahead.type==TokenType::K_GROUPBY){
		nextToken();
		selectCmd->groupBy=parseColumnSpecString();
	}
	::std::unique_ptr<CommandStmt> cmdStmt=::std::make_unique<CommandStmt>();
	cmdStmt->command=::std::move(selectCmd);
	return ::std::move(cmdStmt);

}
void Parser::parseTableDefineField(TableDef& tableDefine,bool& primaryKeySetted){
	if(lookahead.type==TokenType::IDENTIFIER){
		ColumnDef column;
		column.name=getIdentifier("Expected column name"s);
		parseTypeDefine(column.dataType,column.size);
		column.unique=false;
		column.nullable=true;
		while(lookahead.type!=TokenType::P_COMMA&&lookahead.type!=TokenType::P_RPARENT){
			parseColumnConstraint(column.nullable,column.unique);
		}
		tableDefine.columns.push_back(column);
		return;
	}
	if(lookahead.type==TokenType::K_PRIMARY){
		if(primaryKeySetted){
			throw ParseError("Duplicated primary key declaration"s);
		}
		primaryKeySetted=true;
		nextToken();
		eatToken(TokenType::K_KEY,"Expected keyword KEY"s);
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		tableDefine.primaryKeys.push_back(getIdentifier("Expected primary key name"s));
		while(lookahead.type==TokenType::P_COMMA){
			nextToken();
			tableDefine.primaryKeys.push_back(getIdentifier("Expected primary key name"s));
		}
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		return;
	}
	if(lookahead.type==TokenType::K_FOREIGN){
		nextToken();
		eatToken(TokenType::K_KEY,"Expected keyword KEY"s);
		ForeignKeyDef foreignKey;
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		foreignKey.keyName=getIdentifier("Expected key name"s);
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		eatToken(TokenType::K_REFERENCES,"Expected keyword REFERENCES"s);
		foreignKey.refTable=getIdentifier("Expected table name"s);
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		foreignKey.refName=getIdentifier("Expected ref key name"s);
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		tableDefine.foreignKeys.push_back(foreignKey);
		return;
	}
	throw ParseError("Expected column name or keyword PRIMARY/FOREIGN"s);
}
void Parser::parseTypeDefine(DataType& type,size_t& size){
	if(lookahead.type==TokenType::K_INT){
		nextToken();
		type=INT;
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		if(lookahead.type!=TokenType::INT){
			throw ParseError("Expected key length"s);
		}
		size=lookahead.intValue;
		nextToken();
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		return;
	}
	if(lookahead.type==TokenType::K_FLOAT){
		nextToken();
		type=FLOAT;
		size=4;
		return;
	}
	if(lookahead.type==TokenType::K_DATE){
		nextToken();
		type=DATE;
		size=4;
		return;
	}
	if(lookahead.type==TokenType::K_VARCHAR){
		nextToken();
		type=VARCHAR;
		eatToken(TokenType::P_LPARENT,"Expected '('"s);
		if(lookahead.type!=TokenType::INT){
			throw ParseError("Expected key length"s);
		}
		size=lookahead.intValue;
		nextToken();
		eatToken(TokenType::P_RPARENT,"Expected ')'"s);
		return;
	}
	throw ParseError("Expected keyword INT/FLOAT/DATE/VARCHAR"s);
}
void Parser::parseColumnConstraint(bool& nullable,bool& unique){
	if(lookahead.type==TokenType::K_NOT){
		nextToken();
		eatToken(TokenType::K_NULL,"Expected keyword NULL"s);
		if(!nullable){
			throw ParseError("Duplicated not null constraint"s);
		}
		nullable=false;
		return;
	}
	if(lookahead.type==TokenType::K_UNIQUE){
		nextToken();
		if(unique){
			throw ParseError("Duplicated unique constraint"s);
		}
		unique=true;
		return;
	}
	throw ParseError("Expected ')' or ',' or keyword NOT/UNIQUE"s);
}

TableRecord Parser::parseTableRecord(){
	TableRecord tableRecord;
	eatToken(TokenType::P_LPARENT,"Expected '('"s);
	parseTableRecordValue(tableRecord);
	while(lookahead.type==TokenType::P_COMMA){
		nextToken();
		parseTableRecordValue(tableRecord);
	}
	eatToken(TokenType::P_RPARENT,"Expected ')'"s);
	return tableRecord;
}

void Parser::parseTableRecordValue(TableRecord& tableRecord){
	if(lookahead.type==TokenType::K_NULL){
		tableRecord.pushNull(UNKNOWN);
		nextToken();
		return;
	}
	if(lookahead.type==TokenType::INT){
		tableRecord.pushInt(lookahead.intValue);
		nextToken();
		return;
	}
	if(lookahead.type==TokenType::FLOAT){
		tableRecord.pushFloat(lookahead.floatValue);
		nextToken();
		return;
	}
	if(lookahead.type==TokenType::STRING){
		tableRecord.pushVarchar(lookahead.stringValue);
		nextToken();
		return;
	}
	throw ParseError("Expected int, float, string or keyword NULL"s);
}
void Parser::parseWhere(Condition& where){
	where.ands.push_back(parseCondition());
	while(lookahead.type==TokenType::K_AND){
		nextToken();
		where.ands.push_back(parseCondition());
	}
}
BoolExpr Parser::parseCondition(){
	BoolExpr boolExpr;
	parseColumnSpec(boolExpr.tableName,boolExpr.columnName);
	parseOp(boolExpr.op);
	if((boolExpr.op!=BoolExpr::OP_IS_NOT_NULL)&&(boolExpr.op!=BoolExpr::OP_IS_NULL)){
		if(lookahead.type==TokenType::IDENTIFIER){
			boolExpr.rhsAttr=parseColumnSpecString();
			return boolExpr;
		}
		if(lookahead.type==TokenType::INT){
			boolExpr.rhsValue=::std::to_string(lookahead.intValue);
			nextToken();
			return boolExpr;
		}
		if(lookahead.type==TokenType::FLOAT){
			boolExpr.rhsValue=::std::to_string(lookahead.floatValue);
			nextToken();
			return boolExpr;
		}
		if(lookahead.type==TokenType::STRING){
			boolExpr.rhsValue=lookahead.stringValue;
			nextToken();
			return boolExpr;
		}
		throw ParseError("Expected int, float, string or column specification"s);
	}
	return boolExpr;
}
void Parser::parseColumnSpec(::std::string& table,::std::string& column){
	::std::string name1=getIdentifier("Expected column specification"s);
	if(lookahead.type==TokenType::P_DOT){
		table=name1;
		nextToken();
		column=getIdentifier("Expected column name"s);
	}else{
		table="";
		column=name1;
	}
}

::std::string Parser::parseColumnSpecString(){
	::std::string table;
	::std::string column;
	parseColumnSpec(table,column);
	if(table==""){
		return column;
	}else{
		return table+"."+column;
	}
}
void Parser::parseOp(BoolExpr::Operator& op){
	if(lookahead.type==TokenType::P_EQ){
		nextToken();
		op=BoolExpr::OP_EQ;
		return;
	}
	if(lookahead.type==TokenType::P_NE){
		nextToken();
		op=BoolExpr::OP_NE;
		return;
	}
	if(lookahead.type==TokenType::P_L){
		nextToken();
		op=BoolExpr::OP_LT;
		return;
	}
	if(lookahead.type==TokenType::P_G){
		nextToken();
		op=BoolExpr::OP_GT;
		return;
	}
	if(lookahead.type==TokenType::P_LE){
		nextToken();
		op=BoolExpr::OP_LE;
		return;
	}
	if(lookahead.type==TokenType::P_GE){
		nextToken();
		op=BoolExpr::OP_GE;
		return;
	}
	if(lookahead.type==TokenType::K_LIKE){
		nextToken();
		op=BoolExpr::OP_LIKE;
		return;
	}
	if(lookahead.type==TokenType::K_IS){
		nextToken();
		if(lookahead.type==TokenType::K_NOT){
			nextToken();
			eatToken(TokenType::K_NULL,"Expected keyword NULL"s);
			op=BoolExpr::OP_IS_NOT_NULL;
			return;
		}
		eatToken(TokenType::K_NULL,"Expected keyword NULL"s);
		op=BoolExpr::OP_IS_NULL;
		return;
	}
	throw ParseError("Expected operator, IS NULL or IS NOT NULL"s);
}