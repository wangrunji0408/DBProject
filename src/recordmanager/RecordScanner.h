#ifndef RECORDSCANNER_H
#define RECORDSCANNER_H

#include "recordmanager/Record.h"

class Table;
class RecordScanner{
	Table* table;
public:
	RecordScanner(Table* table);
	~RecordScanner();
	Record getNext();
	bool hasNext();
};

#endif //RECORDSCANNER_H