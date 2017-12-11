//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_EXCEPTIONS_H
#define DBPROJECT_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>

struct ParseError: std::exception {

};

struct ExecuteError: std::exception {
	std::string info;

	explicit ExecuteError(std::string info) : info(std::move(info)) {}
	const char *what() const override { return info.c_str(); }
};

#endif //DBPROJECT_EXCEPTIONS_H
