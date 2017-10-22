#ifndef RECORDSCANNER_H
#define RECORDSCANNER_H

#include "recordmanager/Record.h"

class RecordScanner{
public:
	RecordScanner();
	~RecordScanner();
	Record getNext();
	bool hasNext();
};

#endif //RECORDSCANNER_H