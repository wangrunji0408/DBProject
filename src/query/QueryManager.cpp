//
// Created by 王润基 on 2017/12/11.
//

#include <algorithm>
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
	if(cmd.froms.size() == 1)
		return selectFromOne(cmd);
	if(cmd.froms.size() == 2)
		return selectFromTwo(cmd);
	return selectFromMany(cmd);
}

SelectResult QueryManager::selectFromOne(const Select &cmd) const {
	auto tableName = cmd.froms[0];
	auto table = database.getTable(tableName);
	// check selects
	auto selects = vector<string>();
	if(cmd.selects.size() == 1 && cmd.selects[0] == "*")
			selects = {"*"};
		else
			for(auto const& fullname: cmd.selects) {
				auto t = getTableName(fullname);
				auto c = getColName(fullname);
				if(t != tableName && !t.empty())
					throw NameNotExistError("table", t);
				selects.push_back(c);
			}
	// check wheres
	for(auto const& expr: cmd.where.ands)
		if(expr.tableName != tableName && !expr.tableName.empty())
			throw NameNotExistError("table", expr.tableName);
	return table->select(selects, cmd.where);
}

SelectResult QueryManager::selectFromTwo(const Select &cmd) const {
	auto relatedCols = getRelatedCols(cmd.where);

	for(auto const& expr: cmd.where.ands) {
		auto table = database.getTable(expr.tableName);
		auto def = table->getDef();
		if(!expr.rhsAttr.empty()
		   && expr.op == BoolExpr::OP_EQ
		   && def.primaryKeys.size() == 1
		   && def.primaryKeys[0] == expr.columnName) {
			// find t1.pk == t2.foreign

			// select from t2
			auto table2Name = getTableName(expr.rhsAttr);
			auto result2 = selectOne(cmd, relatedCols, table2Name);

			// complete selects
			auto const& table1Name = expr.tableName;
			auto ss = relatedCols[table1Name];
			for(auto const& fullname: cmd.selects)
				if(getTableName(fullname) == table1Name)
					ss.insert(getColName(fullname));
			auto selects = std::vector<std::string>(ss.begin(), ss.end());
			// filter wheres
			auto cond = Condition();
			cond.ands.push_back(expr);
			for(auto const& expr: cmd.where.ands)
				if(expr.tableName == table1Name && expr.rhsAttr.empty())
					cond.ands.push_back(expr);
			auto& e = cond.ands[0];
			e.rhsAttr = "";

			auto result = SelectResult();
			int indexDataId = result2.indexOf(expr.rhsAttr);

			// foreach find in t1 then join
			for(auto const& record: result2.records) {
				auto indexData = record.getDataAtCol(indexDataId);
				e.rhsValue = std::to_string(*(int*)indexData.data()); // TODO support other type (now only INT)
				// select
				auto result1 = table->select(selects, cond);
				if(result1.records.size() > 1)
					throw std::runtime_error("WTF! size > 1");
				if(result.colNames.empty())
					result.colNames = concat(result1.colNames, result2.colNames);
				if(result1.records.size() == 1)
					result.records.push_back(TableRecord::concat(result1.records[0], record));
			}
			auto crossCond = Condition();
			for(auto const& expr: cmd.where.ands)
				if(!expr.rhsAttr.empty())
					crossCond.ands.push_back(expr);
			filter(result, crossCond);
			select(result, cmd.selects);
			return result;
		}

	}
	// can not optimize
	return selectFromMany(cmd);
}

SelectResult QueryManager::selectFromMany(const Select &cmd) const {
	auto relatedCols = getRelatedCols(cmd.where);

	auto results = std::vector<SelectResult>();
	results.reserve(cmd.froms.size());
	for(const auto &tableName: cmd.froms) {
		results.push_back(selectOne(cmd, relatedCols, tableName));
	}

	// sort results by size
	auto order = std::vector<int>(results.size());
	for(int i=0; i<results.size(); ++i)
		order[i] = i;
	std::sort(order.begin(), order.end(), [&](int i, int j) {
		return results[i].records.size() < results[j].records.size();
	});

	// filter cross conditions
	auto restExprs = Condition();
	for(auto const& expr: cmd.where.ands)
		if(!expr.rhsAttr.empty())
			restExprs.ands.push_back(expr);

	// join
	auto result = results[order[0]];
	for(int i=1; i<cmd.froms.size(); ++i) {
		result = join(result, results[order[i]]);

		auto tempExprs = std::move(restExprs.ands);
		auto cond = Condition();
		for(auto const& expr: tempExprs) {
			auto lhsId = result.indexOf(expr.tableName + "." + expr.columnName);
			auto rhsId = result.indexOf(expr.rhsAttr);
			if(lhsId != -1 && rhsId != -1)
				cond.ands.push_back(expr);
			else
				restExprs.ands.push_back(expr);
		}
		filter(result, cond);
	}
	select(result, cmd.selects);

	if(!restExprs.ands.empty()) {
		throw NameNotExistError("column", "(where)");
	}

	return result;
}

SelectResult QueryManager::selectOne(const Select &cmd, const map<string, set<string>> &relatedCols, const string &tableName) const {
	auto table = database.getTable(tableName);
	// filter selects
	auto ss = relatedCols.count(tableName) == 1?
			  relatedCols.at(tableName):
			  std::set<string>();
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
	return table->select(selects, cond);
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
		int id = result.indexOf(name);
		if(id == -1)
			throw NameNotExistError("colunm", name);
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

	auto lhsId = result.indexOf(expr.tableName + "." + expr.columnName);
	auto rhsId = result.indexOf(expr.rhsAttr);
	if(lhsId == -1)
		throw NameNotExistError("column", expr.tableName + "." + expr.columnName);
	if(rhsId == -1)
		throw NameNotExistError("column", expr.rhsAttr);
	auto type = result.records[0].getTypeAtCol(lhsId);
	auto rType = result.records[0].getTypeAtCol(rhsId);
	if(type != rType)
		throw ValueTypeError(type, rType);

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
