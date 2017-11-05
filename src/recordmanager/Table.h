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

class Table{
	friend class RecordManager;
	friend class RecordScanner;
	RecordManager& recordManager;
	Database& database;
	[[deprecated("Use Page class from database")]]
	BufPageManager* bufPageManager;
	::std::string name;
	size_t recordLength;
	size_t maxRecordPerPage;
	const int tablePageID;
	int firstDataPageID;
	void deleteData();
	void recoverMetadata();
	Table(RecordManager& recordManager,::std::string name,int tablePageID);
public:
	size_t getRecordLength()const;
	Record getRecord(RID const&);
	RID insertRecord(const uchar*  data);
	void deleteRecord(RID const&);
	void updateRecord(Record const&);
	RecordScanner iterateRecords();
};

#endif //TABLE_H