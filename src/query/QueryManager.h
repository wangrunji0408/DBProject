//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_QUERYMANAGER_H
#define DBPROJECT_QUERYMANAGER_H


#include <system/Database.h>
#include <ast/Command.h>
#include <table/TableMetaPage.h>
#include <set>
#include <map>

class QueryManager {
	Database& database;
	static SelectResult join(SelectResult const& a, SelectResult const& b);
	static void filter(SelectResult& result, Condition const& cond);
	static void select(SelectResult& result, std::vector<std::string> const& colNames);
	static std::function<bool(TableRecord const&)> makePredict(
			SelectResult const& result, BoolExpr const& expr);
	static std::function<bool(TableRecord const&)> makePredict(
			SelectResult const& result, Condition const& cond);
	static std::map<std::string, std::set<std::string>>
			getRelatedCols(Condition const& cond);
	SelectResult selectOne(const Select &cmd, const map<string, set<string>> &relatedCols, const string &tableName) const;
	SelectResult selectFromOne(const Select &cmd) const;
	SelectResult selectFromTwo(const Select &cmd) const;
	SelectResult selectFromMany(const Select &cmd) const;
public:
	QueryManager(Database &database);
	void execute(Command const& cmd);
	SelectResult select(Select const& cmd);
	void update(Update const& cmd);
	void delete_(Delete const& cmd);
	void insert(Insert const& cmd);
};


#endif //DBPROJECT_QUERYMANAGER_H
