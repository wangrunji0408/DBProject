#ifndef SQLEXECUTOR_H
#define SQLEXECUTOR_H

#include <vector>
#include <memory>
#include <ostream>
#include "ast/Statement.h"
#include "system/DatabaseManager.h"

class SQLExecutor{
	::std::unique_ptr<DatabaseManager> dbManager;
public:
	SQLExecutor(){
		dbManager=make_unique<DatabaseManager>();
	}
	void executeSQL(::std::vector<::std::unique_ptr<Statement>> program,::std::ostream& output);
};

#endif //SQLEXECUTOR_H