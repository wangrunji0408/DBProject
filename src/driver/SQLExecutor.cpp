#include <vector>
#include <memory>
#include <ostream>
#include <stdexcept>
#include "driver/SQLExecutor.h"

void SQLExecutor::executeSQL(::std::vector<::std::unique_ptr<Statement>> program,::std::ostream& output){
	for(::std::unique_ptr<Statement>& stmt:program){
		switch(stmt->getType()){
		case StatementType::SHOW_DATABASES:
			throw ::std::runtime_error("Not implemented");
		case StatementType::CREATE_DATABASE:
			dbManager->createDatabase(dynamic_cast<CreateDatabaseStmt&>(*stmt).database);
			break;
		case StatementType::DROP_DATABASE:
			{
				bool needRestore=false;
				::std::string previousName;
				::std::string deletedName=dynamic_cast<DropDatabaseStmt&>(*stmt).database;
				if(dbManager->getCurrentDatabase()!=nullptr){
					previousName=dbManager->getCurrentDatabase()->getName();
					if(previousName!=deletedName){
						needRestore=true;
					}
				}
				dbManager->useDatabase(deletedName);
				dbManager->deleteCurrentDatabase();
				if(needRestore){
					dbManager->useDatabase(previousName);
				}
			}
			break;
		case StatementType::USE_DATABASE:
			dbManager->useDatabase(dynamic_cast<UseDatabaseStmt&>(*stmt).database);
			break;
		case StatementType::SHOW_TABLES:
			throw ::std::runtime_error("Not implemented");
		case StatementType::SHOW_TABLE_SCHEMA:
			throw ::std::runtime_error("Not implemented");
		case StatementType::CREATE_TABLE:
			dbManager->getCurrentDatabase()->createTable(dynamic_cast<CreateTableStmt&>(*stmt).define);
			break;
		case StatementType::DROP_TABLE:
			dbManager->getCurrentDatabase()->deleteTable(dynamic_cast<DropTableStmt &>(*stmt).table);
			break;
		case StatementType::CREATE_INDEX:
			dbManager->getCurrentDatabase()->createIndex(dynamic_cast<CreateIndexStmt&>(*stmt).table,dynamic_cast<CreateIndexStmt&>(*stmt).column);
			break;
		case StatementType::DROP_INDEX:
			dbManager->getCurrentDatabase()->deleteIndex(dynamic_cast<DropIndexStmt&>(*stmt).table,dynamic_cast<DropIndexStmt&>(*stmt).column);
			break;
		default:
			throw ::std::runtime_error("Unknown statement type");
		}
	}
}
