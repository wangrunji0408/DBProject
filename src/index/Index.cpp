//
// Created by 王润基 on 2017/10/24.
//

#include <system/Database.h>

Index::Index(IndexManager &manager, int rootPageID):
	entityLists(manager.entityLists), database(manager.database), rootPageID(rootPageID)
{
	auto rootPage = database.getPage(rootPageID);
	auto root = (IndexPage*)rootPage.getDataReadonly();
	compare = root->makeCompare();
	id = root->indexID;
	keyLength = root->keyLength;
	keyridBuf = new char[root->keyLength + sizeof(RID)];
}

Index::~Index()
{
	delete[] keyridBuf;
}

void Index::loadToBuf(const void *pData, RID const &rid) const
{
	memcpy(keyridBuf, pData, static_cast<size_t>(keyLength));
	memcpy(keyridBuf + keyLength, &rid, sizeof(RID));
}

bool Index::equals(const void *data1, const void *data2) const {
	return !compare(data1, data2) && !compare(data2, data1);
}


void Index::insertEntry(int nodePageID) {
	auto nodePage = database.getPage(nodePageID);
	auto node = (IndexPage*)nodePage.getDataMutable();
	if(node->leaf)
	{
		int pos = node->lowerBound(keyridBuf, compare);
		if(pos < node->size && equals(keyridBuf, node->refKey(pos)))
			throw std::runtime_error("Key and RID already exist.");
		node->insert(pos, keyridBuf);
	}
	else
	{
		int pos = std::max(0, node->upperBound(keyridBuf, compare) - 1);
		int sonPageID = node->refPageID(pos);
		insertEntry(sonPageID);
		if(pos == 0 && compare(keyridBuf, node->refKey(0)))	// update min key
			std::memcpy(node->refKey(0), keyridBuf, node->keyLength + sizeof(RID));
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

void Index::insertEntry(const void *pData, const RID &rid)
{
	if(iteratorCount > 0)
		throw std::runtime_error("Can not modify when iterating");
	loadToBuf(pData, rid);
	insertEntry(rootPageID);
	if(this->needSplit)
	{
		auto rootPage = database.getPage(rootPageID);
		auto root = (IndexPage*)rootPage.getDataReadonly();
		auto newRootPage = database.acquireNewPage();
		auto newRoot = (IndexPage*)newRootPage.getDataMutable();
		newRoot->makeRootPage(root->indexID, root->keyType, root->keyLength, false);
		newRoot->insert(0, root->refKey(0), rootPageID);
		newRoot->insert(1, this->midKey, this->otherPageID);
		updateRoot(newRootPage.pageId);
	}
}

void Index::updateRoot(int rootPageID) {
	this->rootPageID = rootPageID;
	database.indexManager->resetRootPageID(id, rootPageID);
}


void Index::fixDelete(IndexPage *node, int leftPos) {
	auto leftPageID = node->refPageID(leftPos);
	auto rightPageID = node->refPageID(leftPos + 1);
	auto leftPage = database.getPage(leftPageID);
	auto rightPage = database.getPage(rightPageID);
	auto left = (IndexPage*)leftPage.getDataMutable();
	auto right = (IndexPage*)rightPage.getDataMutable();
	if(left->size + right->size < node->capacity) { // need merge
		left->mergeFromRight(right);
		node->remove(leftPos + 1);
	} else { // need average
		left->averageFromRight(right);
		// update right key in node
		std::memcpy(node->refKey(leftPos + 1), right->refKey(0), node->keyLength + sizeof(RID));
	}
}

void Index::deleteEntry(int nodePageID) {
	auto nodePage = database.getPage(nodePageID);
	auto node = (IndexPage*)nodePage.getDataMutable();
	if(node->leaf)
	{
		int pos = node->lowerBound(keyridBuf, compare);
		if(pos == node->size || !equals(keyridBuf, node->refKey(pos)))
			throw std::runtime_error("Key and RID not exist.");
		node->remove(pos);
	}
	else
	{
		int pos = std::max(0, node->upperBound(keyridBuf, compare) - 1);
		int sonPageID = node->refPageID(pos);
		deleteEntry(sonPageID);
		std::memcpy(node->refKey(pos), this->minKey, node->keyLength + sizeof(RID));
		if(this->needMerge) {
			if(node->size == 1) { // no brother, this is root
				updateRoot(sonPageID);
				database.releasePage(nodePageID);
				return;
			}
			int leftPos = pos + 1 < node->size? pos: pos - 1;
			fixDelete(node, leftPos);
		}
	}
	this->minKey = node->refKey(0);
	this->needMerge = node->size < node->capacity / 2;
}

void Index::deleteEntry(const void *pData, RID const &rid) {
	if(iteratorCount > 0)
		throw std::runtime_error("Can not modify when iterating");
	loadToBuf(pData, rid);
	deleteEntry(rootPageID);
}

RID Index::lowerBound(const void *pData, RID const &rid) const {
	auto page = database.getPage(rootPageID);
	auto node = (IndexPage*)page.getDataReadonly();
	auto compare = node->makeCompare();
	loadToBuf(pData, rid);
	while(true)
	{
		if(node->leaf){
			int slotID = node->lowerBound(keyridBuf, compare);
			if(slotID == node->size) {
				int pageID = node->nextPageID;
				if(pageID == 0)
					return RID(-1, -1);	// is end
				else
					return RID(node->nextPageID, 0);	// first in next page
			}
			return RID(page.pageId, slotID);
		}
		int pageID = node->refPageID(std::max(0, node->upperBound(keyridBuf, compare) - 1));
		page = database.getPage(pageID);
		node = (IndexPage*)page.getDataReadonly();
	}
}

bool Index::containsEntry(const void *pData, RID const &rid) const {
	auto page = database.getPage(rootPageID);
	auto node = (IndexPage*)page.getDataReadonly();
	auto compare = node->makeCompare();
	loadToBuf(pData, rid);
	while(true)
	{
		if(node->leaf){
			int slotID = node->lowerBound(keyridBuf, compare);
			if(slotID == node->size)
				return false;
			auto key = node->refKey(slotID);
			return equals(key, keyridBuf);
		}
		int pageID = node->refPageID(std::max(0, node->upperBound(keyridBuf, compare) - 1));
		page = database.getPage(pageID);
		node = (IndexPage*)page.getDataReadonly();
	}
}

bool Index::containsEntry(const void *pData) const {
	return lowerBound(pData, RID(0,0)) != lowerBound(pData, RID(-1,-1));
}

void Index::print(int pageID) const {
	static int indent;
	if(pageID == 0)
		pageID = rootPageID;
	auto page = database.getPage(pageID);
	auto node = (IndexPage*)page.getDataReadonly();
	for(int i=0; i<node->size; ++i)
	{
		for(int k=0; k<indent; ++k)
			std::cerr << "  ";
		std::cerr << *(int*)node->refKey(i) << ' ' << node->refRID(i) << std::endl;
		if(!node->leaf)
			indent++, print(node->refPageID(i)), indent--;
	}
	if(pageID == rootPageID)
		std::cerr << std::endl;
}

IndexIterator Index::begin() const {
	auto page = database.getPage(rootPageID);
	auto node = (IndexPage*)page.getDataReadonly();
	while(!node->leaf)
	{
		page = database.getPage(node->refPageID(0));
		node = (IndexPage*)page.getDataReadonly();
	}
	return IndexIterator((Index&)*this, page, 0);
}

IndexIterator Index::lowerBound(const void *pData) const {
	auto pos = lowerBound(pData, RID(0,0));
	if(pos == RID(-1,-1))	// end
		return IndexIterator((Index&)*this, database.getPage(0), -1);
	auto page = database.getPage(pos.pageId);
	return IndexIterator((Index&)*this, page, pos.slotId);
}

IndexIterator Index::upperBound(const void *pData) const {
	auto pos = lowerBound(pData, RID(-1,-1));
	if(pos == RID(-1,-1))	// end
		return IndexIterator((Index&)*this, database.getPage(0), -1);
	auto page = database.getPage(pos.pageId);
	return IndexIterator((Index&)*this, page, pos.slotId);
}
