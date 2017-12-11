//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_DATATYPE_H
#define DBPROJECT_DATATYPE_H

#include <string>

enum DataType {
	UNKNOWN = 0,
	INT = 1,
	CHAR = 2,
	VARCHAR = 3,
	FLOAT = 4,
	DATE = 5
};

inline void parse(const std::string &str, unsigned char* buf, DataType type, int size) {
	switch(type) {
		case UNKNOWN:
			throw std::runtime_error("Can not parse UNKNOWN type");
		case INT:
			*(int*)buf = std::stoi(str);
			return;
		case CHAR:
		case VARCHAR:
			if(str.length() > size)
				throw std::runtime_error("String too long");
			str.copy((char*)buf, str.length(), 0);
			return;
		case FLOAT:
			*(float*)buf = std::stof(str);
			return;
		case DATE:
			throw std::runtime_error("Not implemented!");
	}
}

#endif //DBPROJECT_DATATYPE_H
