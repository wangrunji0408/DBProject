#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <cstddef>

class Table;
class DatabaseManager;

class Database{
	DatabaseManager& databaseManager;
	const int fileId;
	Database(DatabaseManager& db,int fileId):databaseManager(db),fileId(fileId){}
public:
	~Database();
	void createTable(::std::string name,size_t recordLength);
	Table* getTable(::std::string name);
	void deleteTable(Table* table);
};

#endif //DATABASE_H