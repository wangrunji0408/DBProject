#ifndef TABLE_H
#define TABLE_H

#include <cstddef>
#include <functional>
#include "recordmanager/Record.h"
#include "recordmanager/RID.h"
#include "recordmanager/RecordScanner.h"
#include "filesystem/utils/pagedef.h"

struct RID;
struct Record;
class RecordScanner;

class Table{
public:
	size_t getRecordLength()const;
	Record getRecord(const RID);
	const RID insertRecord(BufType data);
	void deleteRecord(const RID);
	void updateRecord(const Record);
	RecordScanner iterateRecords(::std::function<const Record(bool)> filter);
};

#endif //TABLE_H