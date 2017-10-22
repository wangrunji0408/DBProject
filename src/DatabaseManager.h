#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <memory>
#include "filesystem/bufmanager/BufPageManager.h"
#include "filesystem/fileio/FileManager.h"
#include "Database.h"

class DatabaseManager{
	::std::unique_ptr<FileManager> fileManager;
	::std::unique_ptr<BufPageManager> bufPageManager;
	::std::unique_ptr<Database> currentDatabase;
	DatabaseManager(){
		fileManager=::std::make_unique<FileManager>()
		bufPageManager=::std::make_unique<BufPageManager>(fileManager.get());
	};
public:
	void createDatabase(::std::string name);
	void useDatabase(::std::string name);
	Database* getCurrentDatabase();
	void deleteCurrentDatabase();
};

#endif //DATABASEMANAGER_H