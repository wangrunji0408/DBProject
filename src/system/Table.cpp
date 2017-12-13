//
// Created by 王润基 on 2017/12/13.
//

#include "Table.h"
#include "TableMetaPage.h"
#include "Database.h"

Table::Table(const int id, const int metaPageID, Database const& database) :
		id(id), metaPageID(metaPageID), database(database) {}

TableDef Table::getDef() const {
	auto metaPage = database.getPage(metaPageID);
	auto meta = (TableMetaPage*)metaPage.getDataReadonly();
	return meta->toDef(database);
}
