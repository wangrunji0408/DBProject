//
// Created by 王润基 on 2017/10/26.
//

#include "IndexIterator.h"
#include "systemmanager/Database.h"

IndexIterator::IndexIterator(Index &index, Page page, int slotID):
	index(index), slotID(slotID), page(page)
{
	index.iteratorCount++;
}

IndexIterator::~IndexIterator() {
	index.iteratorCount--;
}

void *IndexIterator::getKey() {
	auto node = (IndexPage*)page.getDataReadonly();
	return node->refKey(slotID);
}

RID IndexIterator::getRID() {
	auto node = (IndexPage*)page.getDataReadonly();
	return node->refRID(slotID);
}

bool IndexIterator::moveNext() {
	if(end)
		return false;
	auto node = (IndexPage*)page.getDataReadonly();
	slotID++;
	if(slotID == node->size)
	{
		if(node->nextPageID == 0)
		{
			end = true;
			return false;
		}
		page = index.database.getPage(node->nextPageID);
		slotID = 0;
	}
	return true;
}
