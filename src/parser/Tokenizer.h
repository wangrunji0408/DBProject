#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstddef>
#include <string>

enum class TokenType{
	EOS,
	IDENTIFIER,
	STRING,
	INT,
	FLOAT,
	//keywords
	K_AND,
	K_CREATE,
	K_DATABASE,
	K_DATABASES,
	K_DATE,
	K_DELETE,
	K_DESC,
	K_DROP,
	K_FLOAT,
	K_FOREIGN,
	K_FROM,
	K_GROUPBY,
	K_INDEX,
	K_INSERT,
	K_INT,
	K_INTO,
	K_IS,
	K_LIKE,
	K_KEY,
	K_NOT,
	K_NULL,
	K_PRIMARY,
	K_REFERENCES,
	K_SELECT,
	K_SET,
	K_SHOW,
	K_TABLE,
	K_TABLES,
	K_UNIQUE,
	K_UPDATE,
	K_USE,
	K_VALUES,
	K_VARCHAR,
	K_WHERE,
	//punctuations
	P_LPARENT,
	P_RPARENT,
	P_SEMICOLON,
	P_COMMA,
	P_DOT,
	P_STAR,
	P_EQ,
	P_NE,
	P_GE,
	P_G,
	P_L,
	P_LE,
};

struct Token{
	TokenType type;
	int intValue;
	double floatValue;
	::std::string stringValue;
};

class Tokenizer{
	const ::std::string text;
	size_t pos=0;
public:
	Tokenizer(const ::std::string text):text(text){}
	Token next();
};

#endif //TOKENIZER_H