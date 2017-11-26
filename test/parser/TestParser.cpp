#include "../TestBase.h"
#include "parser/Parser.h"
#include "ast/Statement.h"

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
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString(";;;\n;\n");
	ASSERT_EQ(statements.size(),0);
}

TEST_F(TestParser, HandleShowStatements)
{
	::std::vector<::std::unique_ptr<Statement>> statements=Parser::parseString("show databases;show tables");
	ASSERT_EQ(statements.size(),2);
	ASSERT_EQ(statements[0]->getType(),StatementType::SHOW_DATABASES);
	ASSERT_EQ(statements[1]->getType(),StatementType::SHOW_TABLES);
}

}