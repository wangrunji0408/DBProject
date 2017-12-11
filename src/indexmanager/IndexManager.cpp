//
// Created by 王润基 on 2017/10/24.
//

#include "IndexManager.h"
#include "systemmanager/Database.h"
#include "SysIndexPage.h"

IndexManager::IndexManager(Database &database):
		database(database), sysIndexPage(database.getPage(Database::SYSINDEX_PAGEID)),
		entityLists(database)
{

}

int IndexManager::createIndex(int tablePageID, DataType keyType, short keyLength) {
	if(keyType == UNKNOWN)
		throw std::runtime_error("Unknown key type");
	if(keyLength <= 0)
		throw std::runtime_error("Key length invalid");
	if(keyLength > 8000)
		throw std::runtime_error("Key length too large");

	auto sysindex = (SysIndexPage*)sysIndexPage.getDataMutable();
	auto rootPage = database.acquireNewPage();

	int indexID = sysindex->acquireNewIndex();
	auto& indexInfo = sysindex->indexInfo[indexID];
	indexInfo.rootPageID = rootPage.pageId;
	indexInfo.tablePageID = tablePageID;
	indexInfo.keyType = keyType;
	indexInfo.keyLength = keyLength;
	sysindex->indexCount ++;

	auto root = (IndexPage*)rootPage.getDataMutable();
	root->makeRootPage(indexID, keyType, keyLength);
	return indexID;
}

void IndexManager::deleteIndex(int indexID) {
	auto sysindex = (SysIndexPage*)sysIndexPage.getDataMutable();
	if(indexID < 0 || indexID >= SysIndexPage::MAX_INDEX_NUM || !sysindex->indexInfo[indexID].used)
		throw std::runtime_error("Index ID not exist.");
	sysindex->indexInfo[indexID].used = false;
}

std::unique_ptr<Index> IndexManager::getIndex(int indexID) {
	auto sysindex = (SysIndexPage*)sysIndexPage.getDataReadonly();
	if(indexID < 0 || indexID >= SysIndexPage::MAX_INDEX_NUM || !sysindex->indexInfo[indexID].used)
		throw std::runtime_error("Index ID not exist.");
	std::unique_ptr<Index> index;
	index.reset(new Index(*this, sysindex->indexInfo[indexID].rootPageID));
	return index;
}

void IndexManager::resetRootPageID(int indexID, int pageID) {
	auto sysindex = (SysIndexPage*)sysIndexPage.getDataMutable();
	if(indexID < 0 || indexID >= SysIndexPage::MAX_INDEX_NUM || !sysindex->indexInfo[indexID].used)
		throw std::runtime_error("Index ID not exist.");
	sysindex->indexInfo[indexID].rootPageID = pageID;
}

IndexEntityLists* IndexManager::getEntityLists() {
	return &entityLists;
}
