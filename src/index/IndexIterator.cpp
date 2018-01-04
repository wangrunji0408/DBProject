//
// Created by 王润基 on 2017/10/26.
//

#include "IndexIterator.h"
#include "system/Database.h"

IndexIterator::IndexIterator(Index &index, Page page, int slotID):
	index(index), slotID(slotID), page(page)
{
	if(slotID == -1)
		end = true;
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

bool operator==(const IndexIterator &lhs, const IndexIterator &rhs) {
	if(lhs.end == rhs.end && lhs.end)
		return true;
	return lhs.end == rhs.end &&
		   lhs.slotID == rhs.slotID &&
		   lhs.page == rhs.page;
}

bool operator!=(const IndexIterator &lhs, const IndexIterator &rhs) {
	return !(rhs == lhs);
}

bool IndexIterator::isEnd() const {
	return end;
}

IndexIterator::IndexIterator(IndexIterator const &it):
		index(it.index), page(it.page)
{
	end = it.end;
	slotID = it.slotID;
	index.iteratorCount++;
}
