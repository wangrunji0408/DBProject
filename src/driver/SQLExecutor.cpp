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
			throw ::std::runtime_error("Not implemented");
		case StatementType::DROP_DATABASE:
			throw ::std::runtime_error("Not implemented");
		case StatementType::USE_DATABASE:
			throw ::std::runtime_error("Not implemented");
		case StatementType::SHOW_TABLES:
			throw ::std::runtime_error("Not implemented");
		case StatementType::SHOW_TABLE_SCHEMA:
			throw ::std::runtime_error("Not implemented");
		case StatementType::CREATE_TABLE:
			throw ::std::runtime_error("Not implemented");
		case StatementType::DROP_TABLE:
			throw ::std::runtime_error("Not implemented");
		case StatementType::CREATE_INDEX:
			throw ::std::runtime_error("Not implemented");
		case StatementType::DROP_INDEX:
			throw ::std::runtime_error("Not implemented");
		default:
			throw ::std::runtime_error("Unknown statement type");
		}
	}
}
