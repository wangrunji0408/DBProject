//
// Created by 王润基 on 2017/10/26.
//

#include "IndexIterator.h"
#include "systemmanager/Database.h"

IndexIterator::IndexIterator(Database &database, Page page, int slotID):
	database(database), slotID(slotID), page(page)
{

}

bool IndexIterator::hasNext() const {
	return !end;
}

void *IndexIterator::getNext() {
	if(end)
		throw ::std::runtime_error("There is no more entry");
	auto node = (IndexPage*)page.getDataReadonly();
	auto ret = node->refKey(slotID);
	slotID++;
	if(slotID == node->size)
	{
		if(node->nextPageID == 0)
			end = true;
		else
		{
			page = database.getPage(node->nextPageID);
			slotID = 0;
		}
	}
	return ret;
}
