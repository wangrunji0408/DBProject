//
// Created by 王润基 on 2017/12/17.
//

#ifndef DBPROJECT_TABLERECORD_H
#define DBPROJECT_TABLERECORD_H

#include <bitset>
#include "TableMetaPage.h"

typedef unsigned char uchar;
typedef std::vector<uchar> Data;

class TableRecord {
	std::vector<DataType> types;
	std::vector<Data> datas;
public:
	int size() const;
	bool isNullAtCol(int i) const;
	Data const& getDataAtCol(int i) const;
	DataType getTypeAtCol(int i) const;
	TableRecord& pushInt(int x);
	TableRecord& pushFloat(float x);
	TableRecord& pushString(std::string const& s);
	TableRecord& pushDate(std::string const& date);
	TableRecord& pushNull(DataType type);
	static TableRecord fromString(std::vector<DataType> const& types,
								  std::vector<std::string> const& values);
};


#endif //DBPROJECT_TABLERECORD_H
