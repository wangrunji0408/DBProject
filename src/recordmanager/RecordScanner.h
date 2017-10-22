#ifndef RECORDSCANNER_H
#define RECORDSCANNER_H

#include "recordmanager/Record.h"

class RecordScanner{
	RecordScanner();
	~RecordScanner();
public:
	Record* next();
};

#endif //RECORDSCANNER_H