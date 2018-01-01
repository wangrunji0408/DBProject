//
// Created by 王润基 on 2017/12/11.
//

#include <table/TableMetaPage.h>
#include <ast/Exceptions.h>
#include "QueryManager.h"

QueryManager::QueryManager(Database &database) : database(database) {}

void QueryManager::execute(Command const &cmd) {
	switch(cmd.getType()) {
		case CMD_INSERT:
			insert((Insert const&)cmd); break;
		case CMD_UPDATE:
			update((Update const&)cmd); break;
		case CMD_DELETE:
			delete_((Delete const&)cmd); break;
		case CMD_SELECT:
			select((Select const&)cmd); break;
	}
}

inline bool startWith(string const& str, string const& sub) {
	return sub.size() <= str.size() && str.substr(0, sub.size()) == sub;
}
inline string getTableName(string const& str) {
	return str.substr(0, str.find('.'));
}
inline string getColName(string const& str) {
	return str.substr(str.find('.') + 1);
}

SelectResult QueryManager::select(Select const &cmd) {
	if(cmd.froms.empty())
		throw ExecuteError("Froms is empty");
	if(cmd.froms.size() == 1) {
		auto table = database.getTable(cmd.froms[0]);
		return table->select(cmd.selects, cmd.where);
	}
	auto result = SelectResult();
	bool first = true;
	for(const auto &tableName: cmd.froms) {
		auto table = database.getTable(tableName);
		// filter selects
		auto selects = std::vector<std::string>();
		for(auto const& fullname: cmd.selects)
			if(startWith(fullname, tableName))
				selects.push_back(getColName(fullname));
		// filter wheres
		auto cond = Condition();
		for(auto const& expr: cmd.where.ands)
			if(expr.tableName == tableName && expr.rhsAttr.empty())
				cond.ands.push_back(expr);
		// select and join
		auto result0 = table->select(selects, cond);
		result = first? result0: join(result, result0);
		first = false;
	}
	return result;
}

void QueryManager::update(Update const &cmd) {
	auto table = database.getTable(cmd.tableName);
	table->update(cmd.sets, cmd.where);
}

void QueryManager::delete_(Delete const &cmd) {
	auto table = database.getTable(cmd.tableName);
	table->delete_(cmd.where);
}

void QueryManager::insert(Insert const &cmd) {
	auto table = database.getTable(cmd.tableName);
	table->insert(cmd.records);
}

SelectResult QueryManager::join(SelectResult const &a, SelectResult const& b) {
	auto c = SelectResult();
	c.colNames = concat(a.colNames, b.colNames);
	c.records.reserve(a.records.size() * b.records.size());
	for(auto const& r1: a.records)
		for(auto const& r2: b.records)
			c.records.push_back(TableRecord::concat(r1, r2));
	return c;
}
