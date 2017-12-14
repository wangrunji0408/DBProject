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

struct OneValueError {
	int valueNum;
	RecordValue value;
	int attrNum;
	std::string attrName;

	OneValueError(int valueNum, const RecordValue &value, int attrNum, const std::string &attrName) : valueNum(
			valueNum), value(value), attrNum(attrNum), attrName(attrName) {}
};

struct ValueError: ExecuteError {
	std::vector<OneValueError> errors;
	ValueError(const std::vector<OneValueError> &errors) : errors(errors) {}
};

struct NullValueError: OneValueError {
	NullValueError(int valueNum, const RecordValue &value, int attrNum, const std::string &attrName)
			: OneValueError(valueNum, value, attrNum, attrName) {}
};

struct NotUniqueError: OneValueError {
	NotUniqueError(int valueNum, const RecordValue &value, int attrNum, const std::string &attrName)
			: OneValueError(valueNum, value, attrNum, attrName) {}
};

struct ValueSizeError: OneValueError {
	ValueSizeError(int valueNum, const RecordValue &value)
			: OneValueError(valueNum, value, 0, "") {}
};

#endif //DBPROJECT_EXCEPTIONS_H
