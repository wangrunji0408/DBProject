#ifndef RECORD_H
#define RECORD_H

#include "filesystem/utils/pagedef.h"
#include "RID.h"
#include <iostream>

struct Record{
	RID recordID;
	unsigned char* data=nullptr;
	~Record(){
		//TODO: find a way to do something with the memory here
	}
	template <class T>
	T& getDataRef() const { return *(T*)data; }
};

#endif //RECORD_H