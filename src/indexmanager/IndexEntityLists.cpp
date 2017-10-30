//
// Created by 王润基 on 2017/10/30.
//

#include <recordmanager/RID.h>
#include "IndexEntityLists.h"
#include <systemmanager/Database.h>

IndexEntityLists::IndexEntityLists(Database &database):
	database(database)
{
	// Get or Create linked-list table
	try {
		elTable = database.recordManager->getTable(ENTITYLIST_TABLE_NAME);
	} catch (std::exception const& e) {
		database.recordManager->createTable(ENTITYLIST_TABLE_NAME, sizeof(ELNode));
		elTable = database.recordManager->getTable(ENTITYLIST_TABLE_NAME);
	}
}

RID IndexEntityLists::createList() const {
	ELNode node = {RID(), RID()};	// just a empty head node
	return elTable->insertRecord((const uchar*)&node);
}

void IndexEntityLists::deleteList(RID const &headRID) const {
	for(RID nodeRID = headRID; nodeRID != RID(); )
	{
		auto nextRID = elTable->getRecord(nodeRID).getDataRef<ELNode>().nextRID;
		elTable->deleteRecord(nodeRID);
		nodeRID = nextRID;
	}
}

void IndexEntityLists::removeEntity(RID const &dataRID, RID const &headRID) const {
	auto lastRecord = elTable->getRecord(headRID);
	while(true)
	{
		auto& lastNode = lastRecord.getDataRef<ELNode>();
		RID nodeRID = lastNode.nextRID;
		if(nodeRID == RID())	// empty
			throw std::runtime_error("Entity not found in the given list.");
		auto nodeRecord = elTable->getRecord(nodeRID);
		auto& node = nodeRecord.getDataRef<ELNode>();
		if(node.dataRID == dataRID)
		{
			lastNode.nextRID = node.nextRID;
			elTable->updateRecord(lastRecord);
			elTable->deleteRecord(nodeRID);
			return;
		}
		lastRecord = nodeRecord;
	}
}

void IndexEntityLists::insertEntity(RID const &dataRID, RID const &headRID) const {
	auto headRecord = elTable->getRecord(headRID);
	auto& head = headRecord.getDataRef<ELNode>();
	auto node = ELNode{dataRID, head.nextRID};
	head.nextRID = elTable->insertRecord((const uchar*)&node);
	elTable->updateRecord(headRecord);
}

bool IndexEntityLists::containsEntity(RID const &dataRID, RID const &headRID) const {
	auto nodeRID = elTable->getRecord(headRID).getDataRef<ELNode>().nextRID;
	for(; nodeRID != RID(); )
	{
		auto& node = elTable->getRecord(nodeRID).getDataRef<ELNode>();
		if(node.dataRID == dataRID)
			return true;
		nodeRID = node.nextRID;
	}
	return false;
}
