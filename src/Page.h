//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_PAGE_H
#define TEST_PAGE_H

#include "filesystem/bufmanager/BufPageManager.h"

class Page {
	friend class Database;

	BufPageManager* bufPageManager;
	const int fileId, pageId;
	int bufIndex;
	uchar* bufData = nullptr;
	bool dirty = false;
	Page(BufPageManager* bufPageManager, int fileId, int pageId);
public:
	const uchar* getDataReadonly() const;
	uchar* getDataMutable();
	~Page();
};

#endif //TEST_PAGE_H
