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

SelectResult QueryManager::select(Select const &cmd) {
	if(cmd.froms.size() == 1) {
		auto table = database.getTable(cmd.froms[0]);
		return table->select(cmd.selects, cmd.where);
	}
	throw std::runtime_error("Not implemented!");
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
