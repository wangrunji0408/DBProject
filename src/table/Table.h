//
// Created by 王润基 on 2017/12/13.
//

#ifndef DBPROJECT_TABLE_H
#define DBPROJECT_TABLE_H

#include <functional>
#include <ast/Command.h>
#include <record/Record.h>
#include "ast/TableDef.h"
#include "TableMetaPage.h"
#include "filesystem/page/Page.h"
#include "TableRecord.h"

class Database;
class RecordSet;

class Table {
	friend class Database;
	const int id;
	const int metaPageID;
	const std::string name;
	Page metaPage;
	TableMetaPage* meta;
	RecordSet* const recordSet;
	Database const& database;
	Table(RecordSet* rs, Database const& database);
	Data toData(TableRecord const& record) const;
	TableRecord toRecord(const uchar* data, std::vector<int> const& ids) const;
	std::string check(TableRecord const& record) const;
	void checkInsertValues(std::vector<TableRecord> const &records) const;
	void filterThenForeach(Condition const& condition, std::function<void(const Record&)> const& process);
	std::pair<bool, std::vector<RID>> selectWithIndex(Condition const& condition) const;
	std::function<bool(const void*)> makePredict(BoolExpr const& expr) const;
	std::function<bool(const void*)> makePredict(Condition const& condition) const;
	std::function<void(const void*)> makeUpdate(SetStmt const& set) const;
	std::function<void(const void*)> makeUpdate(std::vector<SetStmt> const& sets) const;
public:
	TableDef getDef() const;
	int size() const;
	void insert(std::vector<TableRecord> const &values);
	void delete_(Condition const& condition);
	void update(std::vector<SetStmt> const& sets, Condition const& condition);
	SelectResult select(std::vector<std::string> const& selects, Condition const& condition);
};


#endif //DBPROJECT_TABLE_H
