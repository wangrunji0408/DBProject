#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <cstddef>
#include <memory>
#include <recordmanager/RecordManager.h>
#include "indexmanager/IndexManager.h"
#include "Table.h"
#include "Page.h"

class DatabaseManager;
class RecordScanner;

class Database{

	friend class DatabaseManager;
	friend class RecordManager;
	friend class Index::IndexManager;

	DatabaseManager& databaseManager;
//	Index::IndexManager indexManager;
	std::unique_ptr<RecordManager> recordManager;

	const int fileID;
	::std::string name;

	// 抽象页管理操作，封装BufManager，为底层Manager提供接口
	int acquireNewPage();
	Page getPage(int pageId) const;
	void releasePage(int pageID);

	Database(DatabaseManager& db,int fileID,::std::string name):
		databaseManager(db),fileID(fileID),name(name)
	{
		recordManager = std::unique_ptr<RecordManager>(new RecordManager(*this));
	}
public:
	~Database();
	void createTable(::std::string name,size_t recordLength);
	void deleteTable(Table* table);
	Table* getTable(::std::string name);
	void createIndex(std::string tableName, std::string attrName);
	void deleteIndex(std::string tableName, std::string attrName);
	Index::IndexHandle* getIndex(std::string tableName, std::string attrName);
};

#endif //DATABASE_H
