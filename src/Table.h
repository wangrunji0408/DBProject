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

bool any_filter(const Record& record);

class Table{
public:
	size_t getRecordLength()const;
	Record getRecord(RID const&);
	RID insertRecord(BufType data);
	void deleteRecord(RID const&);
	void updateRecord(Record const&);
	RecordScanner iterateRecords(::std::function<bool(const Record&)> filter = any_filter);
};

#endif //TABLE_H