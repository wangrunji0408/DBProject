//
// Created by 王润基 on 2017/12/14.
//

#include <bitset>
#include <regex>
#include <ast/Exceptions.h>
#include "Table.h"
#include "system/Database.h"

inline
std::function<bool(const char*)> makeLikePredict(std::string const& pattern) {
	auto r = pattern;
	r = std::regex_replace(r, std::regex("%"), ".*");
	r = std::regex_replace(r, std::regex("_"), ".");
	auto regex = std::regex(r);
	return [=](const void* str) {
		return std::regex_match((char*)str, regex);
	};
}

std::function<bool(const void *)> Table::makePredict(Condition const &condition) const {
	auto fs = std::vector<std::function<bool(const void *)>>();
	fs.resize(condition.ands.size());
	for(int i=0; i<condition.ands.size(); ++i)
		fs[i] = makePredict(condition.ands[i]);
	return [fs](const void *pData) {
		for(auto const& f: fs)
			if(!f(pData))
				return false;
		return true;
	};
}

std::function<bool(const void *)> Table::makePredict(BoolExpr const &expr) const {
	int colID = meta->getColomnId(expr.columnName);
	if(colID == -1)
		throw NameNotExistError("column", expr.columnName);
	auto const& col = meta->columns[colID];
	if(!expr.rhsAttr.empty())
		throw ExecuteError("Compare with attr is not supported");

#define CHECK_NULL(RET_VALUE)\
	if(((bitset<128>*)((char*)pData + (bitsetOffset)))->test(colID)) return RET_VALUE;
#define TEST_NULL(isNull) [=](const void* pData) {return ((bitset<128>*)((char*)pData + (bitsetOffset)))->test(colID) == (isNull); }

	int offset = col.offset;
	int bitsetOffset = meta->recordLength - (meta->columnSize + 7)/ 8;

#define BASIC_CASE \
	case BoolExpr::OP_EQ: return CMP(==, false);\
	case BoolExpr::OP_NE: return CMP(!=, true);\
	case BoolExpr::OP_LT: return CMP(<, false);\
	case BoolExpr::OP_GT: return CMP(>, false);\
	case BoolExpr::OP_LE: return CMP(<=, false);\
	case BoolExpr::OP_GE: return CMP(>=, false);\
	case BoolExpr::OP_IS_NULL: return TEST_NULL(true);\
	case BoolExpr::OP_IS_NOT_NULL: return TEST_NULL(false);\
	default: throw ExecuteError("Unexpected op case");

	int intValue;
	float floatValue;
	string strValue;
	bool testNull = expr.op == BoolExpr::OP_IS_NULL || expr.op == BoolExpr::OP_IS_NOT_NULL;
	switch(col.dataType) {
		case UNKNOWN:
			throw ExecuteError("UNKNOWN datatype");
		case INT: case DATE:
			if(!testNull)
			intValue = col.dataType == INT?
					   std::stoi(expr.rhsValue):
					   parseDate(expr.rhsValue);
#define CMP(OP, NULL_RET) [=](const void* pData) {CHECK_NULL(NULL_RET) return *(int*)((char*)pData + offset) OP intValue; }
			switch(expr.op) {
				BASIC_CASE
				case BoolExpr::OP_LIKE:
					throw ExecuteError("Operation LIKE is not capable with type INT");
			}
#undef CMP
		case CHAR:
		case VARCHAR:
			if(!testNull)
			strValue = expr.rhsValue;
#define CMP(OP, NULL_RET) [=](const void* pData) {CHECK_NULL(NULL_RET) return 0 OP strValue.compare((char*)pData + offset); }
			switch(expr.op) {
				BASIC_CASE
				case BoolExpr::OP_LIKE:
					auto predict = makeLikePredict(expr.rhsValue);
					return [=](const void* pData) {CHECK_NULL(false) return predict((char*)pData + offset); };
			}
#undef CMP
		case FLOAT:
			if(!testNull)
			floatValue = std::stof(expr.rhsValue);
#define CMP(OP, NULL_RET) [=](const void* pData) {CHECK_NULL(NULL_RET) return *(float*)((char*)pData + offset) OP floatValue;}
			switch(expr.op) {
				BASIC_CASE
				case BoolExpr::OP_LIKE:
					throw ExecuteError("Operation LIKE is not capable with type FLOAT");
			}
#undef CMP
	}
#undef BASIC_CASE
#undef TEST_NULL
}

std::function<void(const void *)> Table::makeUpdate(const vector<SetStmt> &sets) const {
	auto fs = std::vector<std::function<void(const void *)>>();
	fs.resize(sets.size());
	for(int i=0; i<sets.size(); ++i)
		fs[i] = makeUpdate(sets[i]);
	return [fs](const void *pData) {
		for(auto const& f: fs)
			f(pData);
	};
}

std::function<void(const void *)> Table::makeUpdate(const SetStmt &set) const {
	int colID = meta->getColomnId(set.columnName);
	if(colID == -1)
		throw NameNotExistError("column", set.columnName);
	auto const& col = meta->columns[colID];

	int offset = col.offset;
	int bitsetOffset = meta->recordLength - (meta->columnSize + 7)/ 8;

	if(set.value.empty()) {
		if(!col.nullable)
			throw NullValueError();
		return [=](const void * pData) {
			((bitset<128>*)((char*)pData + bitsetOffset))->set(colID);
		};
	}

#define RESET_NULLABLE ((bitset<128>*)((char*)pData + bitsetOffset))->reset(colID)
	int intValue;
	float floatValue;
	string strValue;
	const void* data;
	std::function<void(const void *)> ret;

	switch(col.dataType) {
		default:
			throw ExecuteError("UNKNOWN datatype");
		case INT: case DATE:
			intValue = col.dataType == INT?
					   std::stoi(set.value):
					   parseDate(set.value);
			data = &intValue;
			ret = [=](const void * pData) {
				*(int*)((char*)pData + offset) = intValue;
				RESET_NULLABLE;
			};
		case CHAR:
		case VARCHAR:
			strValue = set.value;
			data = strValue.data();
			ret = [=](const void * pData) {
				std::strncpy((char*)pData + offset, strValue.c_str(), col.size);
				RESET_NULLABLE;
			};
		case FLOAT:
			floatValue = std::stof(set.value);
			data = &floatValue;
			ret = [=](const void * pData) {
				*(float*)((char*)pData + offset) = floatValue;
				RESET_NULLABLE;
			};
	}
#undef RESET_NULLABLE

	if(col.foreignTableID != -1) {
		// check foreign key
		auto foreignTable = database.getTable(col.foreignTableID);
		auto indexID = foreignTable->meta->columns[col.foreignColumnID].indexID;
		if(indexID == -1)
			throw std::runtime_error("Foreign key without an index is not supported");
		auto index = database.getIndexManager()->getIndex(indexID);
		if(!index->containsEntry(data))
			throw ForeignKeyNotExistError();
	}
	return ret;
}