//
// Created by 王润基 on 2017/10/25.
//

#include <cstring>
#include <stdexcept>
#include "DatabaseMetaPage.h"

void DatabaseMetaPage::check() const {
#define ASSERT_THROW(condition) \
	if(!(condition)) throw std::runtime_error("DatabaseMetaPage check failed: #condition");

	ASSERT_THROW(magicValue == std::string(MAGIC_VALUE));
	ASSERT_THROW(tableCount >= 0 && tableCount <= MAX_TABLE_NUM);
#undef ASSERT_THROW
}

void DatabaseMetaPage::makeInit() {
	std::memset(this, 0, sizeof(DatabaseMetaPage));
	std::memcpy(magicValue, MAGIC_VALUE, 4);
	pageUsedBitset.set(0); // DatabaseMetaPage
	pageUsedBitset.set(1); // SysIndexPage
}

int DatabaseMetaPage::acquireNewPage() {
	for(int i=0; i<MAX_PAGE_NUM; ++i)
		if(!pageUsedBitset[i])
		{
			pageUsedBitset.set(i);
			return i;
		}
	throw ::std::runtime_error("No more page available");
}

DatabaseMetaPage::DatabaseMetaPage() {
	makeInit();
}
