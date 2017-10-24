#ifndef TABLE_H
#define TABLE_H

#include <cstddef>
#include <functional>
#include <string>
#include <recordmanager/RecordManager.h>
#include "recordmanager/Record.h"
#include "recordmanager/RID.h"
#include "recordmanager/RecordScanner.h"
#include "filesystem/utils/pagedef.h"

class RecordManager;

class Table{
	friend class RecordManager;
	friend class RecordScanner;
	RecordManager& database;
	::std::string name;
	size_t recordLength;
	size_t maxRecordPerPage;
	const int tablePageID;
	int firstDataPageID;
	void deleteData();
	void recoverMetadata();
	Table(RecordManager& database,::std::string name,int tablePageID):database(database),name(name),tablePageID(tablePageID){
		recoverMetadata();
	}
public:
	size_t getRecordLength()const;
	Record getRecord(RID const&);
	RID insertRecord(unsigned char* data);
	void deleteRecord(RID const&);
	void updateRecord(Record const&);
	RecordScanner iterateRecords();
};

#endif //TABLE_H