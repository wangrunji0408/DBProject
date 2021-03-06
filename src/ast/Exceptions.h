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
	const char *what() const noexcept override { return info.c_str(); }
};

struct ExecuteError: std::exception {
	std::string info;

	ExecuteError() = default;
	explicit ExecuteError(std::string info) : info(std::move(info)) {}
	const char *what() const noexcept override { return info.c_str(); }
};

struct OneValueError {
	int valueNum;
	TableRecord value;
	int attrNum;
	std::string attrName;

	OneValueError(int valueNum, const TableRecord &value, int attrNum, const std::string &attrName) : valueNum(
			valueNum), value(value), attrNum(attrNum), attrName(attrName) {}
};

struct NameNotExistError: ExecuteError {
	NameNotExistError(std::string const& argument, std::string const& name) :
			ExecuteError("Name '" + name + "' not exist in " + argument) {}
};

struct NullValueError: ExecuteError {
	NullValueError() : ExecuteError("NullValue") {}
};

struct NotUniqueError: ExecuteError {
	NotUniqueError() : ExecuteError("NotUnique") {}
};

struct ValueTypeError: ExecuteError {
	ValueTypeError(DataType expected, DataType actual) :
			ExecuteError("ValueTypeError expect: " + to_string(expected) + " actual: " + to_string(actual)) {}
};

struct ForeignKeyNotExistError: ExecuteError {
	ForeignKeyNotExistError() : ExecuteError("ForeignKeyNotExist") {}
};

#endif //DBPROJECT_EXCEPTIONS_H
