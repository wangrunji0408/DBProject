//
// Created by 王润基 on 2017/10/25.
//

#ifndef TEST_SYSINDEX_H
#define TEST_SYSINDEX_H


struct SysIndexPage {

	struct IndexInfo {
		int rootPageID;
		int tablePageID;
		short keyType;
		short keyLength;
		bool used;
		char reserved[3];
	};

	static const int MAX_INDEX_NUM = 506;

	int indexCount;
	char reserved[96-4];
	IndexInfo indexInfo[MAX_INDEX_NUM];

	void makeInit();
	SysIndexPage();
	int acquireNewIndex();
};


#endif //TEST_SYSINDEX_H
