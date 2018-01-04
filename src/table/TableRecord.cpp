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
	datas.emplace_back((char*)&x, (char*)(&x + 1));
	types.push_back(DataType::INT);
	return *this;
}

TableRecord& TableRecord::pushFloat(float x) {
	datas.emplace_back((char*)&x, (char*)(&x + 1));
	types.push_back(DataType::FLOAT);
	return *this;
}

TableRecord& TableRecord::pushChar(std::string const &s) {
	auto data = Data(s.begin(), s.end());
	data.push_back('\0');
	datas.push_back(data);
	types.push_back(DataType::CHAR);
	return *this;
}

TableRecord& TableRecord::pushVarchar(std::string const &s) {
	auto data = Data(s.begin(), s.end());
	data.push_back('\0');
	datas.push_back(data);
	types.push_back(DataType::VARCHAR);
	return *this;
}

TableRecord& TableRecord::pushDate(std::string const &date) {
	int x = parseDate(date);
	datas.emplace_back((char*)&x, (char*)(&x + 1));
	types.push_back(DataType::DATE);
	return *this;
}

TableRecord &TableRecord::pushNull(DataType type) {
	types.push_back(type);
	datas.emplace_back();
	return *this;
}

TableRecord &TableRecord::push(DataType type, Data const &data) {
	types.push_back(type);
	datas.push_back(data);
	return *this;
}

int TableRecord::size() const {
	return datas.size();
}

TableRecord TableRecord::fromString(std::vector<DataType> const &types, std::vector<std::string> const &values) {
	if(types.size() != values.size())
		throw std::runtime_error("types.size() != values.size()");
	auto record = TableRecord();
	for(int i=0; i<types.size(); ++i) {
		if(values[i].empty()) {
			record.pushNull(types[i]);
			continue;
		}
		switch (types[i]) {
			case UNKNOWN: throw std::runtime_error("UNKNOWN type");
			case INT: record.pushInt(parseInt(values[i])); break;
			case CHAR:record.pushChar(values[i]); break;
			case VARCHAR: record.pushVarchar(values[i]); break;
			case FLOAT: record.pushFloat(parseFloat(values[i])); break;
			case DATE: record.pushDate(values[i]); break;
		}
	}
	return record;
}

bool operator==(const TableRecord &lhs, const TableRecord &rhs) {
	return lhs.types == rhs.types &&
		   lhs.datas == rhs.datas;
}

bool operator!=(const TableRecord &lhs, const TableRecord &rhs) {
	return !(rhs == lhs);
}

template<class T>
inline T to(Data const& data) {
	return *(T*)data.data();
}

std::ostream &operator<<(std::ostream &os, const TableRecord &record) {
	os << "(";
	for(int i=0; i<record.size(); ++i) {
		if(record.isNullAtCol(i)) {
			os << "null";
		} else {
			auto const& data = record.getDataAtCol(i);
			switch (record.getTypeAtCol(i)) {
				case UNKNOWN: os << "UNKNOWN";
				case INT: os << to<int>(data); break;
				case CHAR:
				case VARCHAR: os << '\'' << data.data() << '\''; break;
				case FLOAT: os << to<float>(data); break;
				case DATE: os << dateToString(to<int>(data)); break;
			}
		}
		if(i + 1 != record.size())
			os << ", ";
	}
	return os << ")";
}

TableRecord TableRecord::concat(const TableRecord &a, const TableRecord &b) {
	auto c = TableRecord();
	c.types = ::concat(a.types, b.types);
	c.datas = ::concat(a.datas, b.datas);
	return c;
}

void TableRecord::filter(std::vector<int> const& ids) {
	auto types0 = std::move(types);
	auto datas0 = std::move(datas);
	types.reserve(ids.size());
	datas.reserve(ids.size());
	for(int i: ids) {
		types.push_back(types0[i]);
		datas.push_back(std::move(datas0[i]));
	}
}
