#include "../TestBase.h"
#include "parser/Tokenizer.h"

namespace {

class TestTokenizer : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
	}

	void Reopen() override {
		TestBase::Reopen();
	}
};

TEST_F(TestTokenizer, HandleEmptyString)
{
	Tokenizer tk("   \n\t");
	ASSERT_EQ(tk.next().type,TokenType::EOS);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
}

TEST_F(TestTokenizer, HandleStringToken)
{
	Tokenizer tk("  'str''ing'  'str''ing''");
	Token t=tk.next();
	ASSERT_EQ(t.type,TokenType::STRING);
	ASSERT_EQ(t.stringValue,"str'ing");
	ASSERT_ANY_THROW(tk.next());
}

TEST_F(TestTokenizer, HandleOneLineComment)
{
	Tokenizer tk("   \n\t--nekobox\n 'name'     --pikachu");
	ASSERT_EQ(tk.next().type,TokenType::STRING);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
}

TEST_F(TestTokenizer, HandleMultilineComment)
{
	Tokenizer tk(" /*nya\n nya */  \n'block'\t/*meow\n   meow */  /*/   '***' */\t/*******/\n/*cat  *'box'");
	ASSERT_EQ(tk.next().type,TokenType::STRING);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
}

TEST_F(TestTokenizer, HandleNumerics)
{
	Tokenizer tk("1234 1234. .1234 12.34'a''a'1.2.3.4");
	Token t=tk.next();
	ASSERT_EQ(t.type,TokenType::INT);
	ASSERT_EQ(t.intValue,1234);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::FLOAT);
	ASSERT_DOUBLE_EQ(t.floatValue,1234);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::FLOAT);
	ASSERT_DOUBLE_EQ(t.floatValue,.1234);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::FLOAT);
	ASSERT_DOUBLE_EQ(t.floatValue,12.34);
	ASSERT_EQ(tk.next().type,TokenType::STRING);
	ASSERT_ANY_THROW(tk.next());
}

TEST_F(TestTokenizer, RejectDoubleDot)
{
	Tokenizer tk("    ..    ");
	ASSERT_ANY_THROW(tk.next());
}

TEST_F(TestTokenizer, HandleKeywordAndIdentifier)
{
	Tokenizer tk("AnD CReaTE DATAbaSE dATABASES DATE delete DEsC DRoP fLOAT FOreIGN FRom INdeX InsERT INt iNTO IS lIkE KEy not NULL PRiMArY REfERENCES SELeCT seT sHOw TablE TABLEs uNIque UPDATe USE VAlUES VARchAR wHERE zEbr_a0 _3nyANko");
	ASSERT_EQ(tk.next().type,TokenType::K_AND);
	ASSERT_EQ(tk.next().type,TokenType::K_CREATE);
	ASSERT_EQ(tk.next().type,TokenType::K_DATABASE);
	ASSERT_EQ(tk.next().type,TokenType::K_DATABASES);
	ASSERT_EQ(tk.next().type,TokenType::K_DATE);
	ASSERT_EQ(tk.next().type,TokenType::K_DELETE);
	ASSERT_EQ(tk.next().type,TokenType::K_DESC);
	ASSERT_EQ(tk.next().type,TokenType::K_DROP);
	ASSERT_EQ(tk.next().type,TokenType::K_FLOAT);
	ASSERT_EQ(tk.next().type,TokenType::K_FOREIGN);
	ASSERT_EQ(tk.next().type,TokenType::K_FROM);
	ASSERT_EQ(tk.next().type,TokenType::K_INDEX);
	ASSERT_EQ(tk.next().type,TokenType::K_INSERT);
	ASSERT_EQ(tk.next().type,TokenType::K_INT);
	ASSERT_EQ(tk.next().type,TokenType::K_INTO);
	ASSERT_EQ(tk.next().type,TokenType::K_IS);
	ASSERT_EQ(tk.next().type,TokenType::K_LIKE);
	ASSERT_EQ(tk.next().type,TokenType::K_KEY);
	ASSERT_EQ(tk.next().type,TokenType::K_NOT);
	ASSERT_EQ(tk.next().type,TokenType::K_NULL);
	ASSERT_EQ(tk.next().type,TokenType::K_PRIMARY);
	ASSERT_EQ(tk.next().type,TokenType::K_REFERENCES);
	ASSERT_EQ(tk.next().type,TokenType::K_SELECT);
	ASSERT_EQ(tk.next().type,TokenType::K_SET);
	ASSERT_EQ(tk.next().type,TokenType::K_SHOW);
	ASSERT_EQ(tk.next().type,TokenType::K_TABLE);
	ASSERT_EQ(tk.next().type,TokenType::K_TABLES);
	ASSERT_EQ(tk.next().type,TokenType::K_UNIQUE);
	ASSERT_EQ(tk.next().type,TokenType::K_UPDATE);
	ASSERT_EQ(tk.next().type,TokenType::K_USE);
	ASSERT_EQ(tk.next().type,TokenType::K_VALUES);
	ASSERT_EQ(tk.next().type,TokenType::K_VARCHAR);
	ASSERT_EQ(tk.next().type,TokenType::K_WHERE);
	Token t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"zebr_a0");
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"_3nyanko");
	ASSERT_EQ(tk.next().type,TokenType::EOS);
}

TEST_F(TestTokenizer, HandlePunctuations)
{
	Tokenizer tk("();,=<>>=<=><\n=<<>>= ( = > < <> >= <= , ; . * ).**.*");
	ASSERT_EQ(tk.next().type,TokenType::P_LPARENT);
	ASSERT_EQ(tk.next().type,TokenType::P_RPARENT);
	ASSERT_EQ(tk.next().type,TokenType::P_SEMICOLON);
	ASSERT_EQ(tk.next().type,TokenType::P_COMMA);
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_NE);
	ASSERT_EQ(tk.next().type,TokenType::P_GE);
	ASSERT_EQ(tk.next().type,TokenType::P_LE);
	ASSERT_EQ(tk.next().type,TokenType::P_G);
	ASSERT_EQ(tk.next().type,TokenType::P_L);
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_L);
	ASSERT_EQ(tk.next().type,TokenType::P_NE);
	ASSERT_EQ(tk.next().type,TokenType::P_GE);
	ASSERT_EQ(tk.next().type,TokenType::P_LPARENT);
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_G);
	ASSERT_EQ(tk.next().type,TokenType::P_L);
	ASSERT_EQ(tk.next().type,TokenType::P_NE);
	ASSERT_EQ(tk.next().type,TokenType::P_GE);
	ASSERT_EQ(tk.next().type,TokenType::P_LE);
	ASSERT_EQ(tk.next().type,TokenType::P_COMMA);
	ASSERT_EQ(tk.next().type,TokenType::P_SEMICOLON);
	ASSERT_EQ(tk.next().type,TokenType::P_DOT);
	ASSERT_EQ(tk.next().type,TokenType::P_STAR);
	ASSERT_EQ(tk.next().type,TokenType::P_RPARENT);
	ASSERT_EQ(tk.next().type,TokenType::P_DOT);
	ASSERT_EQ(tk.next().type,TokenType::P_STAR);
	ASSERT_EQ(tk.next().type,TokenType::P_STAR);
	ASSERT_EQ(tk.next().type,TokenType::P_DOT);
	ASSERT_EQ(tk.next().type,TokenType::P_STAR);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
}

TEST_F(TestTokenizer, RejectUnknownCharacter)
{
	Tokenizer tk("/* */#");
	ASSERT_ANY_THROW(tk.next());
}

TEST_F(TestTokenizer, HandleRealWorldUsage)
{
	Tokenizer tk("/*create table*/CREATE TABLE customer(\nid INT(10) NOT NULL,\nfull_name VARCHAR(25) NOT NULL,\n--gender VARCHAR(1) NOT NULL,\nPRIMARY KEY (id)\n);");
	ASSERT_EQ(tk.next().type,TokenType::K_CREATE);
	ASSERT_EQ(tk.next().type,TokenType::K_TABLE);
	Token t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"customer");
	ASSERT_EQ(tk.next().type,TokenType::P_LPARENT);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"id");
	ASSERT_EQ(tk.next().type,TokenType::K_INT);
	ASSERT_EQ(tk.next().type,TokenType::P_LPARENT);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::INT);
	ASSERT_EQ(t.intValue,10);
	ASSERT_EQ(tk.next().type,TokenType::P_RPARENT);
	ASSERT_EQ(tk.next().type,TokenType::K_NOT);
	ASSERT_EQ(tk.next().type,TokenType::K_NULL);
	ASSERT_EQ(tk.next().type,TokenType::P_COMMA);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"full_name");
	ASSERT_EQ(tk.next().type,TokenType::K_VARCHAR);
	ASSERT_EQ(tk.next().type,TokenType::P_LPARENT);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::INT);
	ASSERT_EQ(t.intValue,25);
	ASSERT_EQ(tk.next().type,TokenType::P_RPARENT);
	ASSERT_EQ(tk.next().type,TokenType::K_NOT);
	ASSERT_EQ(tk.next().type,TokenType::K_NULL);
	ASSERT_EQ(tk.next().type,TokenType::P_COMMA);
	ASSERT_EQ(tk.next().type,TokenType::K_PRIMARY);
	ASSERT_EQ(tk.next().type,TokenType::K_KEY);
	ASSERT_EQ(tk.next().type,TokenType::P_LPARENT);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"id");
	ASSERT_EQ(tk.next().type,TokenType::P_RPARENT);
	ASSERT_EQ(tk.next().type,TokenType::P_RPARENT);
	ASSERT_EQ(tk.next().type,TokenType::P_SEMICOLON);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
	ASSERT_EQ(tk.next().type,TokenType::EOS);
}

TEST_F(TestTokenizer, HandleWTFUsage)
{
	Tokenizer tk("'/*''*/--'--987.654 3210\n123.45/*67*/89abcde====;?");
	Token t=tk.next();
	ASSERT_EQ(t.type,TokenType::STRING);
	ASSERT_EQ(t.stringValue,"/*'*/--");
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::FLOAT);
	ASSERT_DOUBLE_EQ(t.floatValue,123.45);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::INT);
	ASSERT_EQ(t.intValue,89);
	t=tk.next();
	ASSERT_EQ(t.type,TokenType::IDENTIFIER);
	ASSERT_EQ(t.stringValue,"abcde");
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_EQ);
	ASSERT_EQ(tk.next().type,TokenType::P_SEMICOLON);
	ASSERT_ANY_THROW(tk.next());
}

}