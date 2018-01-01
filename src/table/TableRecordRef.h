//
// Created by 王润基 on 2018/1/1.
//

#ifndef DBPROJECT_TABLERECORDREF_H
#define DBPROJECT_TABLERECORDREF_H


#include <bitset>
#include "TableMetaPage.h"

typedef unsigned char uchar;
typedef std::vector<uchar> Data;

class TableRecordRef {
public:
	TableMetaPage* meta;
	const unsigned char* pData;
	std::bitset<128>* getNullBitsetPtr() const;
public:
	TableRecordRef(TableMetaPage *meta, const void *pData);
	bool isNullAtCol(int i) const;
	Data getDataAtCol(int i) const;
};


#endif //DBPROJECT_TABLERECORDREF_H
