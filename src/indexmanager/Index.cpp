//
// Created by 王润基 on 2017/10/24.
//

#include <Database.h>
#include "IndexPage.h"

Index::Index(Database &database, int rootPageID):
	database(database), rootPage(database.getPage(rootPageID))
{

}

void Index::insertEntry(void *pData, const RID &rid) {
	IndexPage* indexPage = (IndexPage*)rootPage.getDataMutable();

}

void Index::deleteEntry(void *pData, RID const &rid) {

}
