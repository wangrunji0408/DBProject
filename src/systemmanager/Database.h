#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <cstddef>
#include <memory>
#include <recordmanager/RecordManager.h>
#include "indexmanager/IndexManager.h"
#include "recordmanager/Table.h"
#include "filesystem/page/Page.h"

class DatabaseManager;
class RecordScanner;

class Database{

	friend class DatabaseManager;
	friend class RecordManager;
	friend class IndexManager;
	friend class RecordScanner;
	friend class Table;

	static const int DBMETA_PAGEID = 0;
	static const int SYSINDEX_PAGEID = 1;

	DatabaseManager& databaseManager;
	std::unique_ptr<IndexManager> indexManager;
	std::unique_ptr<RecordManager> recordManager;

	const int fileID;
	::std::string name;

public: // TODO 这里为了测试暂时公开，寻找测试私有函数的解决方案
	// 抽象页管理操作，封装BufManager，为底层Manager提供接口
	Page acquireNewPage();
	Page getPage(int pageId) const;
	void releasePage(int pageID);
	bool isPageUsed(int pageId) const;
	// for test
	IndexManager* getIndexManager() const {return indexManager.get();}

private:

	Database(DatabaseManager& db,int fileID,::std::string name):
		databaseManager(db),fileID(fileID),name(name)
	{
		recordManager = std::unique_ptr<RecordManager>(new RecordManager(*this));
		indexManager = std::unique_ptr<IndexManager>(new IndexManager(*this));
	}
public:
	~Database();
	void createTable(::std::string name,size_t recordLength);
	void deleteTable(Table* table);
	Table* getTable(::std::string name);
	void createIndex(std::string tableName, std::string attrName);
	void deleteIndex(std::string tableName, std::string attrName);
	Index* getIndex(std::string tableName, std::string attrName);
};

#endif //DATABASE_H
