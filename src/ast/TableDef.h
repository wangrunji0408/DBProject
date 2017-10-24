//
// Created by 王润基 on 2017/10/23.
//

#ifndef TEST_TABLEDEF_H
#define TEST_TABLEDEF_H

#include <string>
#include <vector>

enum DataType {
	INT, CHAR, VARCHAR, FLOAT, DATE
};

struct ColomnDef {
	DataType dataType;
	size_t size;
	std::string name;
	bool nullable;
	bool unique;
};

struct ForeignKeyDef {
	std::string keyName;
	std::string refTable;
	std::string refName;
};

struct TableDef {
	std::string name;
	std::vector<ColomnDef> colomns;
	std::string primaryKeyName;
	std::vector<ForeignKeyDef> foreignKeys;
};

#endif //TEST_TABLEDEF_H
