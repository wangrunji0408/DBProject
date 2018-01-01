//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_QUERYMANAGER_H
#define DBPROJECT_QUERYMANAGER_H


#include <system/Database.h>
#include <ast/Command.h>
#include <table/TableMetaPage.h>

class QueryManager {
	Database& database;
	static SelectResult join(SelectResult const& a, SelectResult const& b);
public:
	QueryManager(Database &database);
	void execute(Command const& cmd);
	SelectResult select(Select const& cmd);
	void update(Update const& cmd);
	void delete_(Delete const& cmd);
	void insert(Insert const& cmd);
};


#endif //DBPROJECT_QUERYMANAGER_H
