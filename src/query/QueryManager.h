//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_QUERYMANAGER_H
#define DBPROJECT_QUERYMANAGER_H


#include <systemmanager/Database.h>
#include <ast/Command.h>

class QueryManager {
	Database& database;
public:
	QueryManager(Database &database);
	void execute(CommandDef::Command const& cmd);
	CommandDef::SelectResult select(CommandDef::Select const& cmd);
	void update(CommandDef::Update const& cmd);
	void delete_(CommandDef::Delete const& cmd);
	void insert(CommandDef::Insert const& cmd);
};


#endif //DBPROJECT_QUERYMANAGER_H
