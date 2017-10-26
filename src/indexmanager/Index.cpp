//
// Created by 王润基 on 2017/10/24.
//

#include <systemmanager/Database.h>
#include "IndexPage.h"

Index::Index(Database &database, int rootPageID):
	database(database), rootPageID(rootPageID)
{
	auto root = (IndexPage*)database.getPage(rootPageID).getDataReadonly();
	compare = root->makeCompare();
}

void Index::insertEntry(const void *pData, RID const &rid, int nodePageID) {
	auto node = (IndexPage*)database.getPage(nodePageID).getDataMutable();
	if(node->leaf)
		node->insert(node->lowerBound(pData, compare), pData, rid);
	else
	{
		int pos = std::max(0, node->upperBound(pData, compare) - 1);
		int sonPageID = node->refPageID(pos);
		insertEntry(pData, rid, sonPageID);
		if(pos == 0 && compare(pData, node->refKey(0)))	// update min key
			std::memcpy(node->refKey(0), pData, node->keyLength);
		if(this->needSplit)
			node->insert(node->lowerBound(this->midKey, compare),
						 this->midKey, this->otherPageID);
	}
	this->needSplit = node->size == node->capacity;
	if(this->needSplit)
	{
		auto otherPage = database.acquireNewPage();
		auto otherNode = (IndexPage*)otherPage.getDataMutable();
		node->splitHalfTo(otherNode, otherPage.pageId);
		this->otherPageID = otherPage.pageId;
		this->midKey = otherNode->refKey(0);
	}
}

void Index::insertEntry(const void *pData, const RID &rid) {

	insertEntry(pData, rid, rootPageID);
	if(this->needSplit)
	{
		auto root = (IndexPage*)database.getPage(rootPageID).getDataReadonly();
		auto newRootPage = database.acquireNewPage();
		auto newRoot = (IndexPage*)newRootPage.getDataMutable();
		newRoot->makeRootPage(root->indexID, root->keyType, root->keyLength, false);
		newRoot->insert(0, root->refKey(0), rootPageID);
		newRoot->insert(1, this->midKey, this->otherPageID);
		// update root ID
		rootPageID = newRootPage.pageId;
		database.indexManager->resetRootPageID(root->indexID, newRootPage.pageId);
	}
}

void Index::deleteEntry(const void *pData, RID const &rid) {
	throw std::runtime_error("Delete entry Not Implemented.");
}

RID Index::findEntry(const void *pData) {
	auto node = (IndexPage*)database.getPage(rootPageID).getDataReadonly();
	auto compare = node->makeCompare();
	while(true)
	{
		if(node->leaf){
			int slotID = node->lowerBound(pData, compare);
			auto key = node->refKey(slotID);
			if(!compare(key, pData) && !compare(pData, key)) // ==
				return node->refRID(slotID);
			throw std::runtime_error("Key not found");
		}
		int pageID = node->refPageID(node->upperBound(pData, compare) - 1);
		node = (IndexPage*)database.getPage(pageID).getDataReadonly();
	}
}

RID Index::findEntryIndexPos(const void *pData) {
	auto node = (IndexPage*)database.getPage(rootPageID).getDataReadonly();
	int pageID = rootPageID;
	auto compare = node->makeCompare();
	while(true)
	{
		if(node->leaf)
			return RID(pageID, node->lowerBound(pData, compare));
		pageID = node->refPageID(node->upperBound(pData, compare) - 1);
		node = (IndexPage*)database.getPage(pageID).getDataReadonly();
	}
}

void Index::print(int pageID) {
	if(pageID == 0)
		pageID = rootPageID;
	auto node = (IndexPage*)database.getPage(pageID).getDataReadonly();
	std::cerr << '(';
	if(node->leaf)
		for(int i=0; i<node->size; ++i)
			std::cerr << *(int*)node->refKey(i) << ' ';
	else
		for(int i=0; i<node->size; ++i)
		{
			std::cerr << *(int*)node->refKey(i) << ' ';
			print(node->refPageID(i));
		}
	std::cerr << ')';
	if(pageID == rootPageID)
		std::cerr << std::endl;
}
