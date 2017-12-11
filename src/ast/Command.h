//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_COMMANDDEF_H
#define DBPROJECT_COMMANDDEF_H

#include <string>
#include <vector>

namespace CommandDef {

	struct RecordValue {
		std::vector<std::string> values;
	};

	struct SetStmt {
		std::string columnName;
		std::string value;
	};

	struct BoolExpr {
		enum Operator {
			OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE,
			OP_IS_NULL, OP_IS_NOT_NULL,
			OP_LIKE
		};
		std::string tableName;
		std::string columnName;
		Operator op;
		std::string rhsValue;
		std::string rhsAttr;
	};

	struct Condition {
		std::vector<BoolExpr> ands;
	};

	enum CommandType {
		CMD_SELECT, CMD_INSERT, CMD_UPDATE, CMD_DELETE
	};

	struct Command {
		virtual CommandType getType() const = 0;
	};

	struct Select: Command {
		CommandType getType() const override {
			return CMD_SELECT;
		}
		std::vector<std::string> selects; // special: ["*"]
		std::vector<std::string> froms;
		Condition where;
		std::string groupBy;
	};

	struct Update: Command {
		CommandType getType() const override {
			return CMD_UPDATE;
		}
		std::string tableName;
		std::vector<SetStmt> sets;
		Condition where;
	};

	struct Delete: Command {
		CommandType getType() const override {
			return CMD_DELETE;
		}
		std::string tableName;
		Condition where;
	};

	struct Insert: Command {
		CommandType getType() const override {
			return CMD_INSERT;
		}
		std::string tableName;
		std::vector<RecordValue> records;
	};

	struct Result {

	};

	struct SelectResult: Result {
		std::vector<RecordValue> records;
	};
}

#endif //DBPROJECT_COMMANDDEF_H
