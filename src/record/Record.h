#ifndef RECORD_H
#define RECORD_H

#include "filesystem/utils/pagedef.h"
#include "RID.h"
#include <iostream>

struct Record{
	RID recordID{};
	const unsigned char* data = nullptr;
	template <class T>
	const T& getDataRef() const { return *(const T*)data; }
	Record copyWithNewData(const void *data) const {
		return Record{recordID, (unsigned char*)data};
	}
};

#endif //RECORD_H