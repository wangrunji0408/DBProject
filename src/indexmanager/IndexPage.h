//
// Created by 王润基 on 2017/10/25.
//

#ifndef TEST_INDEXPAGE_H
#define TEST_INDEXPAGE_H


#include <cstring>
#include <functional>
#include <ast/TableDef.h>

struct RID;

/**
 * 定义了索引页的存储结构
 * size = 8192
 * 只能将从Page处获得的data指针转换成该类型指针使用
 */
class IndexPage {
	friend class Index;
	friend class IndexManager;
	typedef std::function<bool(const void*, const void*)> TCompare;
	typedef unsigned short TPointer;

	int tablePageID;	// 0-3
	int indexID;
	int nextPageID;
	short keyType;		// refer to DataType
	short keyLength;
	short slotLength;	// = keyLength + sizeof(int)  Not leaf
						// = keyLength + sizeof(RID)  Leaf
	short size;
	short capacity; 	// = 8096 / slotLength
	bool cluster;
	bool leaf;
	unsigned char reserved[100-28];
	unsigned char records[8096];

	TPointer& refPageID(int i);
	RID& refRID(int i);
	void* refKey(int i);
	const void* refKey(int i) const;

	void check();
	void makeRootPage(int indexID, short keyType, short keyLength, bool leaf = true);

	// MUST ensure compare is made from the same index's node
	int lowerBound(const void* key, TCompare const& compare) const;
	int upperBound(const void* key, TCompare const& compare) const;

	TCompare makeCompare() const;
	void insert(int i, const void* key, RID rid);
	void insert(int i, const void* key, TPointer pageID);
	void remove(int i);
	void splitHalfTo(IndexPage* other, int otherPageID);

	IndexPage() = delete;
	~IndexPage() = delete;

public:
	static bool TEST_MODE;	// set capacity = 4, let split frequently
};


#endif //TEST_INDEXPAGE_H
