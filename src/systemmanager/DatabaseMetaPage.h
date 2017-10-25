//
// Created by 王润基 on 2017/10/25.
//

#ifndef TEST_DATABASEMETAPAGE_H
#define TEST_DATABASEMETAPAGE_H

#include <bitset>

class DatabaseMetaPage {

	friend class DatabaseManager;
	friend class Database;

	static const int MAX_PAGE_NUM = 1 << 15;
	static const int MAX_TABLE_NUM = 30;
	static constexpr const char* MAGIC_VALUE = "DTBS";

	struct TableInfo {
		int metaPageID;
		char name[128-4];
	};

	char magicValue[4];
	char reserved[252-4];
	int tableCount;
	TableInfo tableInfo[MAX_TABLE_NUM];
	std::bitset< MAX_PAGE_NUM > pageUsedBitset;

	DatabaseMetaPage();
	void check() const;
	void makeInit();
	int acquireNewPage();
};


#endif //TEST_DATABASEMETAPAGE_H
