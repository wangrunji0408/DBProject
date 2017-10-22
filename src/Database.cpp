//
// Created by 王润基 on 2017/10/22.
//

#include <string>
#include "Table.h"
#include "Database.h"
#include "DatabaseManager.h"

Database::~Database() {
	this->databaseManager.fileManager->closeFile(this->fileID);
}

void Database::createTable(::std::string name, size_t recordLength) {

}

void Database::deleteTable(Table *table) {

}

Table *Database::getTable(::std::string name) {
	return nullptr;
}
