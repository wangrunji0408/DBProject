#ifndef RECORDSCANNER_H
#define RECORDSCANNER_H

#include "record/Record.h"

class Table;
class RecordScanner{
	Table* table;
	int currentPageId;
	int nextPageId;
	int fieldId;
	bool needUpdate=true;
	bool end=false;
	void update();
public:
	RecordScanner(Table* table);
	~RecordScanner();
	Record getNext();
	bool hasNext();
};

#endif //RECORDSCANNER_H