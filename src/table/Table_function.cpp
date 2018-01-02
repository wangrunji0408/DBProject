//
// Created by 王润基 on 2017/12/14.
//

#include <bitset>
#include "Table.h"
#include "system/Database.h"

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
		throw std::runtime_error("Column name doesn't exist");
	auto const& col = meta->columns[colID];
	if(!expr.rhsAttr.empty())
		throw std::runtime_error("Compare with attr is not supported");

#define TEST_NULL(isNull) [=](const void* pData) {return ((bitset<128>*)((char*)pData + (bitsetOffset)))->test(colID) == (isNull); }

	int offset = col.offset;
	int bitsetOffset = meta->recordLength - (meta->columnSize + 7)/ 8;

#define BASIC_CASE \
	case BoolExpr::OP_EQ: return CMP(==);\
	case BoolExpr::OP_NE: return CMP(!=);\
	case BoolExpr::OP_LT: return CMP(<);\
	case BoolExpr::OP_GT: return CMP(>);\
	case BoolExpr::OP_LE: return CMP(<=);\
	case BoolExpr::OP_GE: return CMP(>=);\
	case BoolExpr::OP_IS_NULL: return TEST_NULL(true);\
	case BoolExpr::OP_IS_NOT_NULL: return TEST_NULL(false);\
	default: throw std::runtime_error("Unexpected op case");

	int intValue;
	float floatValue;
	string strValue;
	switch(col.dataType) {
		case UNKNOWN:
			throw std::runtime_error("UNKNOWN datatype");
		case INT: case DATE:
			intValue = col.dataType == INT?
					   std::stoi(expr.rhsValue):
					   parseDate(expr.rhsValue);
#define CMP(OP) [=](const void* pData) {return *(int*)((char*)pData + offset) OP intValue; }
			switch(expr.op) {
				BASIC_CASE
				case BoolExpr::OP_LIKE:
					throw std::runtime_error("Operation LIKE is not capable with type INT");
			}
#undef CMP
		case CHAR:
		case VARCHAR:
			strValue = expr.rhsValue;
#define CMP(OP) [=](const void* pData) {return 0 OP strValue.compare((char*)pData + offset); }
			switch(expr.op) {
				BASIC_CASE
				case BoolExpr::OP_LIKE:
					return [=](const void* pData) {return false;}; // TODO
			}
#undef CMP
		case FLOAT:
			floatValue = std::stof(expr.rhsValue);
#define CMP(OP) [=](const void* pData) {return *(float*)((char*)pData + offset) OP floatValue;}
			switch(expr.op) {
				BASIC_CASE
				case BoolExpr::OP_LIKE:
					throw std::runtime_error("Operation LIKE is not capable with type FLOAT");
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
		throw std::runtime_error("Column name doesn't exist");
	auto const& col = meta->columns[colID];

	int offset = col.offset;
	int bitsetOffset = meta->recordLength - (meta->columnSize + 7)/ 8;

	if(set.value.empty()) {
		if(!col.nullable)
			throw std::runtime_error("Can not set NULL to non-nullable attribute");
		return [=](const void * pData) {
			((bitset<128>*)((char*)pData + bitsetOffset))->set(colID);
		};
	}

#define RESET_NULLABLE ((bitset<128>*)((char*)pData + bitsetOffset))->reset(colID)
	int intValue;
	float floatValue;
	string strValue;

	switch(col.dataType) {
		case UNKNOWN:
			throw std::runtime_error("UNKNOWN datatype");
		case INT:
			intValue = std::stoi(set.value);
			return [=](const void * pData) {
				*(int*)((char*)pData + offset) = intValue;
				RESET_NULLABLE;
			};
		case CHAR:
		case VARCHAR:
			strValue = set.value;
			return [=](const void * pData) {
				std::strncpy((char*)pData + offset, strValue.c_str(), col.size);
				RESET_NULLABLE;
			};
		case FLOAT:
			floatValue = std::stof(set.value);
			return [=](const void * pData) {
				*(float*)((char*)pData + offset) = floatValue;
				RESET_NULLABLE;
			};
		case DATE:
			throw std::runtime_error("Not implemented!");
	}
#undef RESET_NULLABLE
}