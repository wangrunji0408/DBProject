//
// Created by 王润基 on 2017/12/17.
//

#ifndef DBPROJECT_TABLERECORD_H
#define DBPROJECT_TABLERECORD_H

#include <bitset>
#include <ostream>
#include "TableMetaPage.h"
#include "util/header.h"

template <class T>
inline std::vector<T> concat(std::vector<T> const& v1, std::vector<T> const& v2) {
	auto v = v1;
	v.reserve(v1.size() + v2.size());
	v.insert(v.end(), v2.begin(), v2.end());
	return v;
}

class TableRecord {
	std::vector<DataType> types;
	std::vector<Data> datas;
public:
	int size() const;
	bool isNullAtCol(int i) const;
	Data const& getDataAtCol(int i) const;
	DataType getTypeAtCol(int i) const;
	void filter(std::vector<int> const& ids);
	TableRecord& push(DataType type, Data const& data);
	TableRecord& pushInt(int x);
	TableRecord& pushFloat(float x);
	TableRecord& pushChar(std::string const &s);
	TableRecord& pushVarchar(std::string const &s);
	TableRecord& pushDate(std::string const& date);
	TableRecord& pushNull(DataType type);
	static TableRecord fromString(std::vector<DataType> const& types,
								  std::vector<std::string> const& values);
	friend bool operator==(const TableRecord &lhs, const TableRecord &rhs);
	friend bool operator!=(const TableRecord &lhs, const TableRecord &rhs);
	static TableRecord concat(const TableRecord &lhs, const TableRecord &rhs);
	friend std::ostream &operator<<(std::ostream &os, const TableRecord &record);
};


#endif //DBPROJECT_TABLERECORD_H
