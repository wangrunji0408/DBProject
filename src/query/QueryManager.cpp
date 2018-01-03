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

//inline bool startWith(string const& str, string const& sub) {
//	return sub.size() <= str.size() && str.substr(0, sub.size()) == sub;
//}
inline string getTableName(string const& str) {
	auto dotPos = str.find('.');
	return dotPos == string::npos? "": str.substr(0, dotPos);
}
inline string getColName(string const& str) {
	return str.substr(str.find('.') + 1);
}

SelectResult QueryManager::select(Select const &cmd) {
	if(cmd.froms.empty())
		throw ExecuteError("Froms is empty");
	if(cmd.froms.size() == 1) {
		auto tableName = cmd.froms[0];
		auto table = database.getTable(tableName);
		// check selects
		auto selects = std::vector<std::string>();
		if(cmd.selects.size() == 1 && cmd.selects[0] == "*")
			selects = {"*"};
		else
			for(auto const& fullname: cmd.selects) {
				auto t = getTableName(fullname);
				auto c = getColName(fullname);
				if(t != tableName && !t.empty())
					throw ExecuteError("Invalid table name in 'select'");
				selects.push_back(c);
			}
		// check wheres
		for(auto const& expr: cmd.where.ands)
			if(expr.tableName != tableName && !expr.tableName.empty())
				throw ExecuteError("Invalid table name in 'where'");
		return table->select(selects, cmd.where);
	}
	// filter cross conditions
	auto crossConds = Condition();
	for(auto const& expr: cmd.where.ands)
		if(!expr.rhsAttr.empty())
			crossConds.ands.push_back(expr);

	auto relatedCols = getRelatedCols(cmd.where);

	auto results = std::vector<SelectResult>();
	for(const auto &tableName: cmd.froms) {
		auto table = database.getTable(tableName);
		// filter selects
//		auto selects = std::vector<std::string>();
		auto& ss = relatedCols[tableName];
		for(auto const& fullname: cmd.selects)
			if(getTableName(fullname) == tableName)
				ss.insert(getColName(fullname));
		auto selects = std::vector<std::string>(ss.begin(), ss.end());
		// filter wheres
		auto cond = Condition();
		for(auto const& expr: cmd.where.ands)
			if(expr.tableName == tableName && expr.rhsAttr.empty())
				cond.ands.push_back(expr);
		// select
		results.push_back(table->select(selects, cond));
	}

	// join
	auto result = results[0];
	for(int i=1; i<cmd.froms.size(); ++i) {
		result = join(result, results[i]);
	}

	filter(result, crossConds);
	select(result, cmd.selects);

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

template <class T>
inline int indexOf(std::vector<T> const& vec, T const& value) {
	auto it = std::find(vec.begin(), vec.end(), value);
	return it == vec.end()? -1: it - vec.begin();
}

void QueryManager::filter(SelectResult& a, Condition const &cond) {
	auto predict = makePredict(a, cond);
	auto records = std::move(a.records);
	for(auto& record: records)
		if(predict(record))
			a.records.push_back(std::move(record));
}

void QueryManager::select(SelectResult &result, std::vector<std::string> const &colNames) {
	auto ids = std::vector<int>();
	for(auto const& name: colNames) {
		int id = indexOf(result.colNames, name);
		if(id == -1)
			throw ExecuteError("Colunm not exist");
		ids.push_back(id);
	}
	result.colNames = colNames;
	for(auto& record: result.records)
		record.filter(ids);
}

std::function<bool(TableRecord const &)>
QueryManager::makePredict(SelectResult const& result, Condition const &condition) {
	auto fs = std::vector<std::function<bool(TableRecord const &)>>();
	fs.resize(condition.ands.size());
	for(int i=0; i<condition.ands.size(); ++i)
		fs[i] = makePredict(result, condition.ands[i]);
	return [fs](TableRecord const &record) {
		for(auto const& f: fs)
			if(!f(record))
				return false;
		return true;
	};
}

std::function<bool(TableRecord const &)>
QueryManager::makePredict(SelectResult const& result, BoolExpr const &expr) {

	auto lhsId = indexOf(result.colNames, expr.tableName + "." + expr.columnName);
	auto rhsId = indexOf(result.colNames, expr.rhsAttr);
	if(lhsId == -1 || rhsId == -1)
		throw ExecuteError("Column name doesn't exist");
	auto type = result.records[0].getTypeAtCol(lhsId);
	auto rType = result.records[0].getTypeAtCol(rhsId);
	if(type != rType)
		throw ExecuteError("Type not match");

#define BASIC_CASE \
	case BoolExpr::OP_EQ: return CMP(==);\
	case BoolExpr::OP_NE: return CMP(!=);\
	case BoolExpr::OP_LT: return CMP(<);\
	case BoolExpr::OP_GT: return CMP(>);\
	case BoolExpr::OP_LE: return CMP(<=);\
	case BoolExpr::OP_GE: return CMP(>=);\
	default: throw std::runtime_error("Unexpected op case");
#define CHECK_NULL \
	if(record.isNullAtCol(lhsId) || record.isNullAtCol(rhsId)) return false;

	switch(type) {
		case UNKNOWN:
			throw std::runtime_error("UNKNOWN datatype");
		case INT:
		case DATE:
#define CMP(OP) [=](TableRecord const &record) {\
			CHECK_NULL\
			int v1 = *(int*)record.getDataAtCol(lhsId).data();\
			int v2 = *(int*)record.getDataAtCol(rhsId).data();\
			return v1 OP v2; }

			switch(expr.op) {
				BASIC_CASE
			}
#undef CMP
		case CHAR:
		case VARCHAR:
#define CMP(OP) [=](TableRecord const &record) {\
			CHECK_NULL\
			auto v1 = (char*)record.getDataAtCol(lhsId).data();\
			auto v2 = (char*)record.getDataAtCol(rhsId).data();\
			return strcmp(v1,v2) OP 0; }

			switch(expr.op) {
				BASIC_CASE
			}
#undef CMP
		case FLOAT:
#define CMP(OP) [=](TableRecord const &record) {\
			CHECK_NULL\
			float v1 = *(float*)record.getDataAtCol(lhsId).data();\
			float v2 = *(float*)record.getDataAtCol(rhsId).data();\
			return v1 OP v2; }
			switch(expr.op) {
				BASIC_CASE
			}
#undef CMP
	}
#undef BASIC_CASE
#undef TEST_NULL
}

std::map<std::string, std::set<std::string>>
QueryManager::getRelatedCols(Condition const& cond) {
	auto s = std::map<std::string, std::set<std::string>>();
	for(auto const& expr: cond.ands) {
		s[expr.tableName].insert(expr.columnName);
		if(!expr.rhsAttr.empty()) {
			auto t = getTableName(expr.rhsAttr);
			auto c = getColName(expr.rhsAttr);
			s[t].insert(c);
		}
	}
	return s;
}
