//
// Created by 王润基 on 2017/12/17.
//

#include "TableRecord.h"

bool TableRecord::isNullAtCol(int i) const {
	return datas[i].empty();
}

Data const& TableRecord::getDataAtCol(int i) const {
	return datas[i];
}

DataType TableRecord::getTypeAtCol(int i) const {
	return types[i];
}

TableRecord& TableRecord::pushInt(int x) {
	datas.emplace_back(&x, &x + 1);
	types.push_back(DataType::INT);
	return *this;
}

TableRecord& TableRecord::pushFloat(float x) {
	datas.emplace_back(&x, &x + 1);
	types.push_back(DataType::FLOAT);
	return *this;
}

TableRecord& TableRecord::pushString(std::string const &s) {
	datas.emplace_back(s.begin(), s.end());
	types.push_back(DataType::CHAR);
	return *this;
}

TableRecord& TableRecord::pushDate(std::string const &date) {
	int x = parseDate(date);
	datas.emplace_back(&x, &x + 1);
	types.push_back(DataType::DATE);
	return *this;
}

TableRecord &TableRecord::pushNull(DataType type) {
	types.push_back(type);
	datas.emplace_back();
	return *this;
}

int TableRecord::size() const {
	return datas.size();
}

TableRecord TableRecord::fromString(std::vector<DataType> const &types, std::vector<std::string> const &values) {
	if(types.size() != values.size())
		throw std::runtime_error("types.size() != values.size()");
	auto record = TableRecord();
	record.types = types;
	for(int i=0; i<types.size(); ++i) {
		if(values[i].empty()) {
			record.pushNull(types[i]);
			continue;
		}
		switch (types[i]) {
			case UNKNOWN: throw std::runtime_error("UNKNOWN type");
			case INT: record.pushInt(parseInt(values[i])); break;
			case CHAR:
			case VARCHAR: record.pushString(values[i]); break;
			case FLOAT: record.pushFloat(parseFloat(values[i])); break;
			case DATE: record.pushDate(values[i]); break;
		}
	}
	return record;
}
