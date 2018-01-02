#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <cstddef>
#include <memory>
#include <record/RecordManager.h>
#include "index/IndexManager.h"
#include "record/RecordSet.h"
#include "filesystem/page/Page.h"
#include "table/Table.h"

class QueryManager;
class DatabaseManager;
class RecordScanner;

class Database{

	friend class DatabaseManager;
	friend class RecordManager;
	friend class IndexManager;
	friend class IndexEntityLists;
	friend class RecordScanner;
	friend class TableMetaPage;
	friend class RecordSet;
	friend class Index;

	static const int DBMETA_PAGEID = 0;
	static const int SYSINDEX_PAGEID = 1;

	DatabaseManager& databaseManager;
	std::unique_ptr<IndexManager> indexManager;
	std::unique_ptr<RecordManager> recordManager;
	std::unique_ptr<QueryManager> queryManager;

	const int fileID;
	::std::string name;

public: // TODO 这里为了测试暂时公开，寻找测试私有函数的解决方案
	// 抽象页管理操作，封装BufManager，为底层Manager提供接口
	Page acquireNewPage();
	Page getPage(int pageId) const;
	void releasePage(int pageID);
	bool isPageUsed(int pageId) const;
	// for test
	RecordManager* getRecordManager() const { return recordManager.get(); }
	IndexManager* getIndexManager() const {return indexManager.get();}

private:
	Database(DatabaseManager& db,int fileID,::std::string name);
	int getIndexID(std::string const& tableName, std::string const& attrName) const;
public:
	~Database();
	::std::string getName() const { return name; };
	void createTable(TableDef const& def);
	Table* getTable(std::string const &name) const;
	void deleteTable(std::string const &name);
	void createIndex(std::string const& tableName, std::string const& attrName);
	void deleteIndex(std::string const& tableName, std::string const& attrName);
	std::unique_ptr<Index> getIndex(std::string const& tableName, std::string const& attrName) const;
	void execute(Command const& cmd);
	SelectResult select(Select const& cmd);
	::std::vector<::std::string> getTableNames()const;
};

#endif //DATABASE_H
