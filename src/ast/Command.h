//
// Created by 王润基 on 2017/12/11.
//

#ifndef DBPROJECT_COMMANDDEF_H
#define DBPROJECT_COMMANDDEF_H

#include <string>
#include <vector>
#include <ostream>
#include <table/TableRecord.h>

//namespace CommandDef {

	struct SetStmt {
		std::string columnName;
		std::string value;

		friend std::ostream &operator<<(std::ostream &os, const SetStmt &stmt) {
			os << stmt.columnName << " = " << stmt.value;
			return os;
		}
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
		virtual void output(std::ostream &os) const = 0;
		friend std::ostream &operator<<(std::ostream &os, const Command &command) {
			command.output(os);
			return os;
		}
	};

	struct Select: Command {
		std::vector<std::string> selects; // special: ["*"]
		std::vector<std::string> froms;
		Condition where;
		std::string groupBy;

		CommandType getType() const override {
			return CMD_SELECT;
		}
		void output(std::ostream &os) const override {
			os << "SELECT ...";
			os << "FROM ...";
			os << "WHERE ...";
			os << "GROUPBY ...";
		}
	};

	struct Update: Command {
		std::string tableName;
		std::vector<SetStmt> sets;
		Condition where;

		CommandType getType() const override {
			return CMD_UPDATE;
		}
		void output(std::ostream &os) const override {
			os << "UPDATE ...";
			os << "SET ...";
			os << "WHERE ...";
		}
	};

	struct Delete: Command {
		std::string tableName;
		Condition where;

		CommandType getType() const override {
			return CMD_DELETE;
		}
		void output(std::ostream &os) const override {
			os << "DELETE ...";
			os << "FROM ...";
			os << "WHERE ...";
		}
	};

	struct Insert: Command {
		std::string tableName;
		std::vector<TableRecord> records;

		CommandType getType() const override {
			return CMD_INSERT;
		}
		void output(std::ostream &os) const override {
			os << "INSERT ...";
			os << "VALUES ...";
		}
	};

	struct Result {

	};

	struct SelectResult: Result {
		std::vector<std::string> colNames;
		std::vector<TableRecord> records;

		friend std::ostream &operator<<(std::ostream &os, const SelectResult &result) {
			os << '(';
			for(int i=0; i<result.colNames.size(); ++i) {
				os << result.colNames[i] << (i+1 != result.colNames.size()? ", ": "");
			}
			os << ')' << std::endl;
			for(auto const& record: result.records)
				os << record << std::endl;
			return os;
		}
	};
//}

#endif //DBPROJECT_COMMANDDEF_H
