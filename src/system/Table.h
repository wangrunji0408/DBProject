//
// Created by 王润基 on 2017/12/13.
//

#ifndef DBPROJECT_TABLE_H
#define DBPROJECT_TABLE_H

#include "ast/TableDef.h"

class Database;

class Table {
	friend class Database;
	const int id;
	const int metaPageID;
	Database const& database;
	Table(int id, int metaPageID, Database const& database);
public:
	TableDef getDef() const;

};


#endif //DBPROJECT_TABLE_H
