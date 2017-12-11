//
// Created by 王润基 on 2017/10/23.
//

#ifndef TABLEDEF_H
#define TABLEDEF_H

#include <string>
#include <vector>
#include "DataType.h"

struct ColumnDef {
	std::string name;
	DataType dataType;
	size_t size;
	bool nullable;
	bool unique;

	friend bool operator==(const ColumnDef &lhs, const ColumnDef &rhs) {
		return lhs.name == rhs.name &&
			   lhs.dataType == rhs.dataType &&
			   lhs.size == rhs.size &&
			   lhs.nullable == rhs.nullable &&
			   lhs.unique == rhs.unique;
	}

	friend bool operator!=(const ColumnDef &lhs, const ColumnDef &rhs) {
		return !(rhs == lhs);
	}
};

struct ForeignKeyDef {
	std::string keyName;
	std::string refTable;
	std::string refName;

	friend bool operator==(const ForeignKeyDef &lhs, const ForeignKeyDef &rhs) {
		return lhs.keyName == rhs.keyName &&
			   lhs.refTable == rhs.refTable &&
			   lhs.refName == rhs.refName;
	}

	friend bool operator!=(const ForeignKeyDef &lhs, const ForeignKeyDef &rhs) {
		return !(rhs == lhs);
	}
};

struct TableDef {
	std::string name;
	std::vector<ColumnDef> columns;
	std::vector<ForeignKeyDef> foreignKeys;
	std::vector<std::string> primaryKeys;

	friend bool operator==(const TableDef &lhs, const TableDef &rhs) {
		return lhs.name == rhs.name &&
			   lhs.columns == rhs.columns &&
			   lhs.foreignKeys == rhs.foreignKeys &&
			   lhs.primaryKeys == rhs.primaryKeys;
	}

	friend bool operator!=(const TableDef &lhs, const TableDef &rhs) {
		return !(rhs == lhs);
	}
};

#endif //TABLEDEF_H
