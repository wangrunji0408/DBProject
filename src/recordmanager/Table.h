#ifndef TABLE_H
#define TABLE_H

#include <cstddef>
#include <functional>
#include <string>
#include "RID.h"
#include "Record.h"
#include "RecordScanner.h"

class RecordManager;
class Database;
class BufPageManager;
class TableDef;

class Table{
	friend class RecordManager;
	friend class RecordScanner;
	friend class TableMetaPage;
	RecordManager& recordManager;
	Database& database;
	[[deprecated("Use Page class from database")]]
	BufPageManager* bufPageManager;
	const int id;
	::std::string name;
	size_t recordLength;
	size_t maxRecordPerPage;
	const int tablePageID;
	int firstDataPageID;
	void deleteData();
	void recoverMetadata();
	Table(RecordManager& recordManager,::std::string name,int tablePageID,int id);
public:
	TableDef getDef() const;
	size_t getRecordLength()const;
	Record getRecord(RID const&);
	RID insertRecord(const uchar*  data);
	void deleteRecord(RID const&);
	void updateRecord(Record const&);
	RecordScanner iterateRecords();
};

#endif //TABLE_H