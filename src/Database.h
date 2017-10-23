#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <cstddef>
#include <iostream>

class Table;
class DatabaseManager;

class Database{
	friend class DatabaseManager;
	DatabaseManager& databaseManager;
	const int fileID;
	::std::string name;
	int acquireNewPage();
	void releasePage(int pageID);
	Database(DatabaseManager& db,int fileID,::std::string name):databaseManager(db),fileID(fileID),name(name){}
public:
	~Database();
	void createTable(::std::string name,size_t recordLength);
	Table* getTable(::std::string name);
	void deleteTable(Table* table);
};

#endif //DATABASE_H
