//
// Created by 王润基 on 2017/10/25.
//

#include <stdexcept>
#include <ast/TableDef.h>
#include "IndexPage.h"
#include "../recordmanager/RID.h"
#include <cassert>

int &IndexPage::refPageID(int i) {
	assert(i >= 0 && i < size);
	return *(int*)(records + slotLength * i + keyLength);
}

RID &IndexPage::refRID(int i) {
	assert(i >= 0 && i < size);
	return *(RID*)(records + slotLength * i + keyLength);
}

void *IndexPage::refKey(int i) {
	assert(i >= 0 && i < size);
	return records + slotLength * i;
}

const void* IndexPage::refKey(int i) const {
	assert(i >= 0 && i < size);
	return records + slotLength * i;
}

void IndexPage::checkAndInit() {
#define ASSERT_THROW(condition) \
	if(!(condition)) throw std::runtime_error("Index page check failed: #condition");

	ASSERT_THROW(tablePageID > 0);
	ASSERT_THROW(superPageID > 0);
	ASSERT_THROW(keyType >= 1 && keyType <= 5);
	ASSERT_THROW(slotLength == keyLength + (leaf? sizeof(RID): sizeof(int)));
	ASSERT_THROW(capacity == 8096 / slotLength);
	ASSERT_THROW(size >= 0 && size < capacity);

	compare = makeCompare();
#undef ASSERT_THROW
}

IndexPage::TCompare IndexPage::makeCompare() const {
#define COMPARE(expr) [](const void* pa, const void* pb){return (expr);}
	switch(keyType)
	{
	case INT:
		return COMPARE(*(int*)pa < *(int*)pb);
	case CHAR: case VARCHAR:
		return COMPARE(strcmp((char*)pa, (char*)pb));
	case FLOAT:
		return COMPARE(*(float*)pa < *(float*)pb);
	default:
		throw std::runtime_error("Not supported data type.");
	}
#undef COMPARE
}

int IndexPage::lowerBound(void *key) const {
	// TODO Maybe binary search?
	int i = 1;
	while(i < size && compare(refKey(i), key))
		++i;
	return i-1;
}

void IndexPage::insert(int i, void *key, RID rid) {
	assert(size < capacity);
	assert(i >= 0 && i <= size);
	std::memmove(refKey(i+1), refKey(i), (size_t)(slotLength * (size - i)));
	std::memcpy(refKey(i), key, (size_t)keyLength);
	refRID(i) = rid;
	size++;
}

void IndexPage::insert(int i, void *key, int pageID) {
	assert(size < capacity);
	assert(i >= 0 && i <= size);
	std::memmove(refKey(i+1), refKey(i), (size_t)(slotLength * (size - i)));
	std::memcpy(refKey(i), key, (size_t)keyLength);
	refPageID(i) = pageID;
	size++;
}

void IndexPage::remove(int i) {
	assert(i >= 0 && i < size);
	std::memmove(refKey(i), refKey(i+1), (size_t)(slotLength * (size - i-1)));
	size--;
}

void IndexPage::makeRootPage(int _tablePageID, DataType _keyType, short _keyLength) {
	tablePageID = _tablePageID;
	superPageID = -1;
	keyType = _keyType;
	keyLength = _keyLength;
	slotLength = keyLength + sizeof(RID);
	size = 0;
	capacity = (short)8096 / slotLength;
	cluster = false;
	leaf = true;
	compare = makeCompare();
}


