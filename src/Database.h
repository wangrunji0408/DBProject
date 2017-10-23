#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <cstddef>
#include <memory>
#include "Table.h"

class DatabaseManager;

class Database{
	friend class DatabaseManager;
	friend class Table;
	DatabaseManager& databaseManager;
	::std::unique_ptr<Table> tables[30];
	size_t tableCount;
	const int fileID;
	::std::string name;
	int acquireNewPage();
	void releasePage(int pageID);
	void recoverTables();
	Database(DatabaseManager& db,int fileID,::std::string name):databaseManager(db),fileID(fileID),name(name){
		recoverTables();
	}
public:
	~Database();
	void createTable(::std::string name,size_t recordLength);
	Table* getTable(::std::string name);
	void deleteTable(Table* table);
};

#endif //DATABASE_H
