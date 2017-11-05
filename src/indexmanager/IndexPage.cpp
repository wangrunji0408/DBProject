//
// Created by 王润基 on 2017/10/25.
//

#include <stdexcept>
#include <ast/TableDef.h>
#include "IndexPage.h"
#include "../recordmanager/RID.h"
#include <cassert>

bool IndexPage::TEST_MODE = false;

IndexPage::TPointer &IndexPage::refPageID(int i) {
	assert(i >= 0 && i < size);
	return *(TPointer*)(records + slotLength * i + keyLength + sizeof(RID));
}

RID &IndexPage::refRID(int i) {
	assert(i >= 0 && i < size);
	return *(RID*)(records + slotLength * i + keyLength);
}

void *IndexPage::refKey(int i) {
	assert(i >= 0 && i <= size);
	return records + slotLength * i;
}

const void* IndexPage::refKey(int i) const {
	assert(i >= 0 && i <= size);
	return records + slotLength * i;
}

void IndexPage::check() {
#define ASSERT_THROW(condition) \
	if(!(condition)) throw std::runtime_error("Index page check failed: #condition");

	ASSERT_THROW(tablePageID > 0);
	ASSERT_THROW(indexID >= 0);
	ASSERT_THROW(nextPageID >= 0);
	ASSERT_THROW(keyType >= 1 && keyType <= 5);
	ASSERT_THROW(slotLength == keyLength + sizeof(RID) + (leaf? 0: sizeof(TPointer)));
	ASSERT_THROW(capacity == 8096 / slotLength);
	ASSERT_THROW(size >= 0 && size < capacity);

#undef ASSERT_THROW
}

IndexPage::TCompare IndexPage::makeCompare() const {
#define COMPARE(init,less,equal) \
	[=](const void* pa, const void* pb)\
	{init;\
	 RID const& rida = *(const RID*)((const char*)pa + keyLength);\
	 RID const& ridb = *(const RID*)((const char*)pb + keyLength);\
	 return (less) || ((equal) && rida < ridb);}

	switch(keyType)
	{
	case INT:
		return COMPARE(int c = *(int*)pa - *(int*)pb, c<0, c==0);
	case CHAR: case VARCHAR:
		return COMPARE(int c = strcmp((char*)pa, (char*)pb), c<0, c==0);
	case FLOAT:
		return COMPARE(float c = *(float*)pa - *(float*)pb, c<0, c==0);
	default:
		throw std::runtime_error("Not supported data type.");
	}
#undef COMPARE
}

int IndexPage::lowerBound(const void *key, TCompare const& compare) const {
	// TODO Maybe binary search?
	int i = 0;
	while(i < size && (compare(refKey(i), key))) // leftmost key <= k[i]
		++i;
	return i;
}

int IndexPage::upperBound(const void *key, TCompare const& compare) const {
	// TODO Maybe binary search?
	int i = 0;
	while(i < size && (!compare(key, refKey(i)))) // leftmost key < k[i]
		++i;
	return i;
}

void IndexPage::insert(int i, const void *key) {
	assert(size < capacity);
	assert(i >= 0 && i <= size);
	if(i < size)
		std::memmove(refKey(i+1), refKey(i), (size_t)(slotLength * (size - i)));
	std::memcpy(refKey(i), key, (size_t)keyLength + sizeof(RID));
	size++;
}

void IndexPage::insert(int i, const void *key, TPointer pageID) {
	insert(i, key);
	refPageID(i) = pageID;
}

void IndexPage::remove(int i) {
	assert(i >= 0 && i < size);
	std::memmove(refKey(i), refKey(i+1), (size_t)(slotLength * (size - i-1)));
	size--;
}

void IndexPage::splitHalfTo(IndexPage *other, int otherPageID) {
	std::memcpy(other, this, 96); // copy header
	other->nextPageID = nextPageID;
	nextPageID = otherPageID;
	// [0, size/2) [size/2, size)
	other->size = size - size / (short)2;
	size /= 2;
	std::memcpy(other->records, records + slotLength * size, (size_t)(slotLength * other->size));

}

void IndexPage::makeRootPage(int _indexID, short _keyType, short _keyLength, bool _leaf) {
	std::memset(this, 0, sizeof(IndexPage));
//	tablePageID = _tablePageID;
	indexID = _indexID;
	keyType = _keyType;
	keyLength = _keyLength;
	slotLength = keyLength + sizeof(RID) + (_leaf? 0: sizeof(TPointer));
	size = 0;
	capacity = (short)8096 / slotLength;
	if(TEST_MODE)
		capacity = 4;	// For test. MUST >= 4
	cluster = false;
	leaf = _leaf;
}

void IndexPage::mergeFromRight(IndexPage *other) {
	assert(size + other->size < capacity);
	nextPageID = other->nextPageID;
	std::memcpy(refKey(size), other->refKey(0), slotLength * other->size);
	size += other->size;
}

void IndexPage::averageFromRight(IndexPage *other) {
	int totalSize = size + other->size;
	int r2lSize = totalSize / 2 - size;
	if(r2lSize > 0) {
		std::memcpy(this->refKey(size), other->records, slotLength * r2lSize);
		int newRightSize = other->size - r2lSize;
		std::memmove(other->records, other->refKey(r2lSize), slotLength * newRightSize);
	} else if(r2lSize < 0) {
		int l2rSize = -r2lSize;
		std::memmove(other->refKey(l2rSize), other->records, slotLength * other->size);
		std::memcpy(other->records, this->refKey(size - l2rSize), slotLength * l2rSize);
	}
	size = totalSize / 2;
	other->size = totalSize - size;
}


