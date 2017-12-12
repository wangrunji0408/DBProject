//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_QUERYMANAGER_H
#define DBPROJECT_QUERYMANAGER_H


#include <system/Database.h>
#include <ast/Command.h>
#include <system/TableMetaPage.h>

class QueryManager {
	Database& database;
	void checkInsertValues(TableMetaPage const &meta, CommandDef::Insert const &cmd) const;
	void makeRecordData(uchar* buf, TableMetaPage const& meta, CommandDef::RecordValue const& value) const;
public:
	QueryManager(Database &database);
	void execute(CommandDef::Command const& cmd);
	CommandDef::SelectResult select(CommandDef::Select const& cmd);
	void update(CommandDef::Update const& cmd);
	void delete_(CommandDef::Delete const& cmd);
	void insert(CommandDef::Insert const& cmd);
};


#endif //DBPROJECT_QUERYMANAGER_H
