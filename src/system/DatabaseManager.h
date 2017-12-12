#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <memory>
#include "filesystem/bufmanager/BufPageManager.h"
#include "filesystem/fileio/FileManager.h"
#include "Database.h"
#include "record/Table.h"

class RecordScanner;

class DatabaseManager{
	friend class Database;
	friend class Table; // for codes not using class Page

	::std::unique_ptr<FileManager> fileManager;
	::std::unique_ptr<BufPageManager> bufPageManager;
	::std::unique_ptr<Database> currentDatabase;
	static bool hasInstance;
public:
	DatabaseManager(){
		if(hasInstance)
			throw std::runtime_error("DatabaseManager should be singleton.");
		fileManager=::std::make_unique<FileManager>();
		bufPageManager=::std::make_unique<BufPageManager>(fileManager.get());
		hasInstance = true;
	};
	~DatabaseManager() {
		hasInstance = false;
	}
	void createDatabase(::std::string name);
	void useDatabase(::std::string name);
	Database* getCurrentDatabase();
	void deleteCurrentDatabase();
};

#endif //DATABASEMANAGER_H