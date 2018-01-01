//
// Created by 王润基 on 2017/12/17.
//

#ifndef DBPROJECT_TABLERECORD_H
#define DBPROJECT_TABLERECORD_H

#include <bitset>
#include <ast/Command.h>
#include "TableMetaPage.h"

typedef std::vector<char> Data;

class TableRecord {
	TableMetaPage* meta;
	const unsigned char* pData;
	bool ownData = false;
	std::bitset<128>* getNullBitsetPtr() const;
public:
	TableRecord(TableMetaPage *meta, const void *pData);
	TableRecord(TableMetaPage *meta, RecordValue const& value);
	~TableRecord();

	bool isNullAtCol(int i) const;
	Data getDataAtCol(int i) const;
	Data getData() const;
	const unsigned char* getDataRef() const;
	const unsigned char* getDataRefAtCol(int i) const;
};


#endif //DBPROJECT_TABLERECORD_H
