//
// Created by 王润基 on 2017/10/22.
//

#include <string>
#include <cassert>
#include "record/RecordSet.h"
#include "Database.h"
#include "DatabaseManager.h"
#include "DatabaseMetaPage.h"
#include "query/QueryManager.h"

Database::Database(DatabaseManager& db,int fileID,::std::string name):
	databaseManager(db),fileID(fileID),name(name)
{
	recordManager = std::unique_ptr<RecordManager>(new RecordManager(*this));
	indexManager = std::unique_ptr<IndexManager>(new IndexManager(*this));
	queryManager = std::unique_ptr<QueryManager>(new QueryManager(*this));
}

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

void Database::createTable(TableDef const &def) {
	static TableMetaPage metaPage;
	metaPage.makeFromDef(def, *this);
	recordManager->createSet(def.name, static_cast<size_t>(metaPage.recordLength));
	int metaPageID = recordManager->getSet(def.name)->tablePageID;
	auto buf = getPage(metaPageID).getDataMutable();
	std::memcpy(buf, &metaPage, sizeof(TableMetaPage));

	// Create index for the first primary key
	if(!def.primaryKeys.empty())
		this->createIndex(def.name, def.primaryKeys[0]);
}

void Database::createIndex(std::string const& tableName, std::string const& attrName) {
	auto table = recordManager->getSet(tableName);
	auto meta = (TableMetaPage*)getPage(table->tablePageID).getDataReadonly();
	int colID = meta->getColomnId(attrName);
	if(colID == -1)
		throw std::runtime_error("Attribute name not exist");
	auto& col = meta->columns[colID];
	if(col.indexID != -1)
		throw std::runtime_error("Attribute already has an index");
	col.indexID = (short)(indexManager->createIndex(table->tablePageID, col.dataType, col.size));
}

void Database::deleteIndex(std::string const& tableName, std::string const& attrName) {
	indexManager->deleteIndex(getIndexID(tableName, attrName));
}

std::unique_ptr<Index> Database::getIndex(std::string const& tableName, std::string const& attrName) const {
	return indexManager->getIndex(getIndexID(tableName, attrName));
}

int Database::getIndexID(std::string const& tableName, std::string const& attrName) const {
	auto table = recordManager->getSet(tableName);
	auto meta = (TableMetaPage*)getPage(table->tablePageID).getDataReadonly();
	int colID = meta->getColomnId(attrName);
	if(colID == -1)
		throw std::runtime_error("Attribute name not exist");
	int indexID = meta->columns[colID].indexID;
	if(indexID == -1)
		throw std::runtime_error("This attribute doesn't have index");
	return indexID;
}

Table *Database::getTable(std::string const &name) const {
	return new Table(recordManager->getSet(name), *this);
}

void Database::deleteTable(std::string const &name) {
	recordManager->deleteSet(name);
}

void Database::execute(Command const &cmd) {
	queryManager->execute(cmd);
}

SelectResult Database::select(Select const &cmd) {
	return queryManager->select(cmd);
}

::std::vector<::std::string> Database::getTableNames()const{
	return recordManager->getSetNames();
}

Table *Database::getTable(int id) const {
	return new Table(recordManager->tables[id].get(), *this);
}
