//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_PAGE_H
#define TEST_PAGE_H

#include "filesystem/bufmanager/BufPageManager.h"

class Page {
	friend class Database;

	BufPageManager* const bufPageManager;

	int bufIndex;
	BufType bufData = nullptr;

	Page(BufPageManager* bufPageManager, int fileId, int pageId);
	void loadToBuf();

public:
	friend bool operator==(const Page &lhs, const Page &rhs);
	friend bool operator!=(const Page &lhs, const Page &rhs);
	Page& operator=(Page const& rhs);

	const int fileId, pageId;
	BufType getDataReadonly();
	BufType getDataMutable();
	~Page();
};

#endif //TEST_PAGE_H
