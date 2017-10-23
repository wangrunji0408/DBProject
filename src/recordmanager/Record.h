#ifndef RECORD_H
#define RECORD_H

#include "filesystem/utils/pagedef.h"
#include "RID.h"

struct Record{
	RID recordID;
	unsigned char* data;
	~Record(){
		delete[] data;
	}
	template <class T>
	T& getDataRef() const { return *(T*)data; }
};

#endif //RECORD_H