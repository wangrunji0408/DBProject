#ifndef RECORDSCANNER_H
#define RECORDSCANNER_H

#include "recordmanager/Record.h"

class RecordScanner{
public:
	RecordScanner();
	~RecordScanner();
	Record* next();
};

#endif //RECORDSCANNER_H