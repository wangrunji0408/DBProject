//
// Created by 王润基 on 2017/12/11.
//

#include "QueryManager.h"
using namespace CommandDef;

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

CommandDef::SelectResult QueryManager::select(Select const &cmd) {
	throw std::runtime_error("Not implemented!");
}

void QueryManager::update(Update const &cmd) {
	throw std::runtime_error("Not implemented!");
}

void QueryManager::delete_(Delete const &cmd) {
	throw std::runtime_error("Not implemented!");
}

void QueryManager::insert(Insert const &cmd) {
	throw std::runtime_error("Not implemented!");
}
