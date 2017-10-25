//
// Created by 王润基 on 2017/10/24.
//

#include <systemmanager/Database.h>
#include "IndexPage.h"

Index::Index(Database &database, int rootPageID):
	database(database), rootPage(database.getPage(rootPageID))
{

}

void Index::insertEntry(const void *pData, const RID &rid) {
	RID pos = findEntryIndexPos(pData);
	auto node = (IndexPage*)database.getPage(pos.pageId).getDataMutable();
	node->insert(pos.slotId, pData, rid);
}

void Index::deleteEntry(const void *pData, RID const &rid) {
	throw std::runtime_error("Delete entry Not Implemented.");
}

RID Index::findEntry(const void *pData) {
	auto node = (IndexPage*)rootPage.getDataReadonly();
	auto compare = node->makeCompare();
	while(true)
	{
		int slotID = node->lowerBound(pData, compare);
		if(node->leaf){
			auto key = node->refKey(slotID);
			if(!compare(key, pData) && !compare(pData, key)) // ==
				return node->refRID(slotID);
			throw std::runtime_error("Key not found");
		}
		int pageID = node->refPageID(slotID);
		node = (IndexPage*)database.getPage(pageID).getDataReadonly();
	}
}

RID Index::findEntryIndexPos(const void *pData) {
	auto node = (IndexPage*)rootPage.getDataReadonly();
	int pageID = rootPage.pageId;
	auto compare = node->makeCompare();
	while(true)
	{
		int slotID = node->lowerBound(pData, compare);
		if(node->leaf)
			return RID(pageID, slotID);
		pageID = node->refPageID(slotID);
		node = (IndexPage*)database.getPage(pageID).getDataReadonly();
	}
}
