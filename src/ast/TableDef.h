//
// Created by 王润基 on 2017/10/23.
//

#ifndef TABLEDEF_H
#define TABLEDEF_H

#include <string>
#include <vector>

enum DataType {
	UNKNOWN = 0,
	INT = 1,
	CHAR = 2,
	VARCHAR = 3,
	FLOAT = 4,
	DATE = 5
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

#endif //TABLEDEF_H
