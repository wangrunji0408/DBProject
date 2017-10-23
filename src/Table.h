#ifndef TABLE_H
#define TABLE_H

#include <cstddef>
#include <functional>
#include <string>
#include "recordmanager/Record.h"
#include "recordmanager/RID.h"
#include "recordmanager/RecordScanner.h"
#include "filesystem/utils/pagedef.h"

struct RID;
struct Record;
class RecordScanner;
class Database;
class DatabaseManager;

class Table{
	friend class Database;
	Database& database;
	::std::string name;
	size_t recordLength;
	size_t maxRecordPerPage;
	const int tablePageID;
	int firstDataPageID;
	void deleteData();
	void recoverMetadata();
	Table(Database& database,::std::string name,int tablePageID):database(database),name(name),tablePageID(tablePageID){
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