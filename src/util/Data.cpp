//
// Created by 王润基 on 2018/1/3.
//

#include <cstring>
#include "Data.h"

bool operator<(const Data &lhs, const Data &rhs) {
	if (lhs._size == rhs._size)
		return std::memcmp(lhs.p, rhs.p, lhs._size) < 0;
	return lhs._size < rhs._size;
}

Data::~Data() {
	delete[] p;
}

size_t Data::size() const {
	return _size;
}

Data::Data(size_t size) {
	_size = size;
	p = new char[size];
	std::memset(p, 0, _size);
}

const unsigned char *Data::data() const {
	return (const unsigned char *)p;
}

unsigned char *Data::data() {
	return (unsigned char *)p;
}

unsigned char Data::at(int i) const {
	return (unsigned char)p[i];
}

bool Data::empty() const {
	return _size == 0;
}

Data::Data(Data const &data) {
	_size = data._size;
	p = new char[_size];
	std::memcpy(p, data.p, _size);
}

Data::Data() {
	p = nullptr;
	_size = 0;
}

bool operator==(const Data &lhs, const Data &rhs) {
	return lhs._size == rhs._size && std::memcmp(lhs.p, rhs.p, lhs._size) == 0;
}

Data::Data(const void *begin, const void *end) {
	_size = (char*)end - (char*)begin;
	p = new char[_size];
	std::memcpy(p, begin, _size);
}

Data Data::fromString(std::string s) {
	auto d = Data(s.data(), s.data() + s.size() + 1);
	d.p[s.size()] = 0;
	return d;
}
