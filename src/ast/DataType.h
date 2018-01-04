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

inline std::string to_string(DataType t) {
	switch (t) {
		case UNKNOWN: 	return "UNKNOWN";
		case INT:		return "INT";
		case CHAR:		return "CHAR";
		case VARCHAR:	return "VARCHAR";
		case FLOAT:		return "FLOAT";
		case DATE:		return "DATE";
	}
	return "ERROR";
}

inline int parseInt(const std::string &str) {
	size_t parseEnd;
	int v = std::stoi(str, &parseEnd);
	if(parseEnd != str.length())
		throw std::runtime_error("Failed to parse int");
	return v;
}

inline float parseFloat(const std::string &str) {
	size_t parseEnd;
	float v = std::stof(str, &parseEnd);
	if(parseEnd != str.length())
		throw std::runtime_error("Failed to parse float");
	return v;
}

inline int parseDate(const std::string &str) {
	tm tm{0}, tm_new;
	time_t time_new;
	try {
		tm.tm_year = parseInt(str.substr(0, 4)) - 1900;
		tm.tm_mon = parseInt(str.substr(5, 2)) - 1;
		tm.tm_mday = parseInt(str.substr(8, 2));
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

inline std::string dateToString(time_t x) {
	tm tm;
	localtime_r(&x, &tm);
	return std::to_string(tm.tm_year + 1900)
		   + "/" + std::to_string(tm.tm_mon + 1)
		   + "/" + std::to_string(tm.tm_mday);
}

inline void parse(const std::string &str, unsigned char* buf, DataType type, int size) {
	switch(type) {
		case UNKNOWN:
			throw std::runtime_error("Can not parse UNKNOWN type");
		case INT:
			*(int*)buf = parseInt(str);
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
			*(float*)buf = parseFloat(str);
			return;
		case DATE:
			*(int*)buf = parseDate(str);
			return;
	}
}

#endif //DBPROJECT_DATATYPE_H
