//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_EXCEPTIONS_H
#define DBPROJECT_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>
#include <ostream>
#include "Command.h"

struct ParseError: std::exception {
	std::string info;

	explicit ParseError(std::string info) : info(std::move(info)) {}
	const char *what() const _NOEXCEPT override { return info.c_str(); }
};

struct ExecuteError: std::exception {
	std::string info;

	ExecuteError() = default;
	explicit ExecuteError(std::string info) : info(std::move(info)) {}
	const char *what() const _NOEXCEPT override { return info.c_str(); }
};

struct ValueError {
	int valueNum;
	CommandDef::RecordValue value;
	int attrNum;
	std::string attrName;

	ValueError(int valueNum, const CommandDef::RecordValue &value, int attrNum, const std::string &attrName) : valueNum(
			valueNum), value(value), attrNum(attrNum), attrName(attrName) {}
};

struct NotNullableError: ValueError {
	NotNullableError(int valueNum, const CommandDef::RecordValue &value, int attrNum, const std::string &attrName)
			: ValueError(valueNum, value, attrNum, attrName) {}
};

struct NotUniqueError: ValueError {
	NotUniqueError(int valueNum, const CommandDef::RecordValue &value, int attrNum, const std::string &attrName)
			: ValueError(valueNum, value, attrNum, attrName) {}
};

#endif //DBPROJECT_EXCEPTIONS_H
