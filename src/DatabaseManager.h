#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <memory>
#include "filesystem/bufmanager/BufPageManager.h"
#include "filesystem/fileio/FileManager.h"
#include "Database.h"
#include "Table.h"

class RecordScanner;

class DatabaseManager{
	friend class Database;
	friend class Table;
	friend class RecordScanner;
	::std::unique_ptr<FileManager> fileManager;
	::std::unique_ptr<BufPageManager> bufPageManager;
	::std::unique_ptr<Database> currentDatabase;
public:
	DatabaseManager(){
		fileManager=::std::make_unique<FileManager>();
		bufPageManager=::std::make_unique<BufPageManager>(fileManager.get());
	};
	void createDatabase(::std::string name);
	void useDatabase(::std::string name);
	Database* getCurrentDatabase();
	void deleteCurrentDatabase();
};

#endif //DATABASEMANAGER_H