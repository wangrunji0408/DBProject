//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_DATATYPE_H
#define DBPROJECT_DATATYPE_H

#include <ctime>
#include <stdexcept>
#include <string>

enum DataType {
	UNKNOWN = 0,
	INT = 1,
	CHAR = 2,
	VARCHAR = 3,
	FLOAT = 4,
	DATE = 5
};

inline int parseDate(const std::string &str) {
	tm tm{0}, tm_new;
	time_t time_new;
	try {
		tm.tm_year = std::stoi(str.substr(0, 4)) - 1900;
		tm.tm_mon = std::stoi(str.substr(5, 2)) - 1;
		tm.tm_mday = std::stoi(str.substr(8, 2));
		time_new = mktime(&tm);
		localtime_r(&time_new, &tm_new);
	} catch (std::exception const& e) {
		throw std::runtime_error("Failed to parse date");
	}
	if(tm_new.tm_year != tm.tm_year
			|| tm_new.tm_mon != tm.tm_mon
			|| tm_new.tm_mday != tm.tm_mday)
		throw std::runtime_error("Failed to parse date");
	return static_cast<int>(time_new);
}

inline void parse(const std::string &str, unsigned char* buf, DataType type, int size) {
	switch(type) {
		case UNKNOWN:
			throw std::runtime_error("Can not parse UNKNOWN type");
		case INT:
			*(int*)buf = std::stoi(str);
			if(std::to_string(*(int*)buf) != str)
				throw std::runtime_error("Failed to parse int");
			return;
		case CHAR:
			if(str.length() != size)
				throw std::runtime_error("String length error");
			str.copy((char*)buf, size, 0);
		case VARCHAR:
			if(str.length() > size)
				throw std::runtime_error("String too long");
			str.copy((char*)buf, str.length(), 0);
			return;
		case FLOAT:
			*(float*)buf = std::stof(str);
			if(std::to_string(*(float*)buf) != str)
				throw std::runtime_error("Failed to parse float");
			return;
		case DATE:
			*(int*)buf = parseDate(str);
			return;
	}
}

#endif //DBPROJECT_DATATYPE_H
