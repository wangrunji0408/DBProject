#include "DatabaseManager.h"

void DatabaseManager::createDatabase(::std::string name){
	this->fileManager->createFile((name+".dbf").c_str());
}

void DatabaseManager::useDatabase(::std::string name) {

}

Database *DatabaseManager::getCurrentDatabase() {
	return nullptr;
}

void DatabaseManager::deleteCurrentDatabase() {

}
