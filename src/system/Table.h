//
// Created by 王润基 on 2017/12/13.
//

#ifndef DBPROJECT_TABLE_H
#define DBPROJECT_TABLE_H

#include <ast/Command.h>
#include "ast/TableDef.h"
#include "TableMetaPage.h"

class Database;
class RecordSet;

class Table {
	friend class Database;
	const int id;
	const int metaPageID;
	const std::string name;
	TableMetaPage* meta;
	RecordSet* const recordSet;
	Database const& database;
	Table(RecordSet* rs, Database const& database);
	void checkInsertValues(std::vector<CommandDef::RecordValue> const &values) const;
	void makeRecordData(unsigned char* buf, CommandDef::RecordValue const& value) const;
public:
	TableDef getDef() const;
	void insert(std::vector<CommandDef::RecordValue> const &values);
};


#endif //DBPROJECT_TABLE_H
