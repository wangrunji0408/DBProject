//
// Created by 王润基 on 2018/1/3.
//

#ifndef DBPROJECT_DATA_H
#define DBPROJECT_DATA_H

#include <string>

class Data {
	char* p;
	size_t _size;
public:
	Data();
	Data(size_t size);
	Data(Data const& data);
	Data(const void* begin, const void* end);
	~Data();
	static Data fromInt(int x);
	static Data fromFloat(float x);
	static Data fromString(std::string s);
	size_t size() const;
	bool empty() const;
	const unsigned char* data() const;
	unsigned char* data();
	unsigned char at(int i) const;
	friend bool operator<(const Data &lhs, const Data &rhs);
	friend bool operator==(const Data &lhs, const Data &rhs);
};


#endif //DBPROJECT_DATA_H
