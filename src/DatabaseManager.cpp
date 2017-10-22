#include "DatabaseManager.h"

void DatabaseManager::createDatabase(::std::string name){
	this->fileManager->createFile((name+".dbf").c_str());
}