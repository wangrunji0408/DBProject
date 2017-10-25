//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_INDEXHANDLE_H
#define TEST_INDEXHANDLE_H

#include "../recordmanager/RID.h"

class Index {
public:
	Index();
	~Index();
	void insertEntry(void *pData, RID const& rid);
	void deleteEntry(void *pData, RID const& rid);
};

#endif //TEST_INDEXHANDLE_H
