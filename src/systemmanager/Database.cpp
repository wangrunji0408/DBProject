//
// Created by 王润基 on 2017/10/22.
//

#include <string>
#include <cassert>
#include "recordmanager/Table.h"
#include "Database.h"
#include "DatabaseManager.h"
#include "DatabaseMetaPage.h"

Page Database::acquireNewPage() {
	auto firstPage = getPage(0);
	auto dbMetaInfo = (DatabaseMetaPage*) firstPage.getDataMutable();
	int pageID = dbMetaInfo->acquireNewPage();
	return Page(this->databaseManager.bufPageManager.get(), fileID, pageID);
}

bool Database::isPageUsed(int pageID) const {
	assert(pageID >= 0 && pageID < DatabaseMetaPage::MAX_PAGE_NUM);
	if(pageID == 0)	return true;
	auto firstPage = getPage(0);
	auto dbMetaInfo = (DatabaseMetaPage*) firstPage.getDataReadonly();
	return dbMetaInfo->pageUsedBitset[pageID];
}

void Database::releasePage(int pageID){
	assert(pageID > 0 && pageID < DatabaseMetaPage::MAX_PAGE_NUM);
	auto firstPage = getPage(0);
	auto dbMetaInfo = (DatabaseMetaPage*) firstPage.getDataMutable();
	if(!dbMetaInfo->pageUsedBitset[pageID])
		throw ::std::runtime_error("Double page release");
	dbMetaInfo->pageUsedBitset.reset(pageID);
}

Page Database::getPage(int pageId) const {
	if(!isPageUsed(pageId))
		throw ::std::runtime_error("Try to get a unused page.");
	return Page(this->databaseManager.bufPageManager.get(), fileID, pageId);
}

Database::~Database() {
	this->databaseManager.bufPageManager->close();
	this->databaseManager.fileManager->closeFile(this->fileID);
}

void Database::createTable(::std::string name, size_t recordLength) {
	recordManager->createTable(name, recordLength);
}

void Database::deleteTable(Table *table) {
	recordManager->deleteTable(table);
}

Table *Database::getTable(::std::string name) {
	return recordManager->getTable(name);
}
