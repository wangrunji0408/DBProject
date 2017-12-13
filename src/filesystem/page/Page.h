//
// Created by 王润基 on 2017/10/24.
//

#ifndef TEST_PAGE_H
#define TEST_PAGE_H

#include "filesystem/bufmanager/BufPageManager.h"

class Page {
	friend class Database;

	BufPageManager* const bufPageManager;

	Page(BufPageManager* bufPageManager, int fileId, int pageId);

public:
	friend bool operator==(const Page &lhs, const Page &rhs);
	friend bool operator!=(const Page &lhs, const Page &rhs);
	Page& operator=(Page const& rhs);

	const int fileId, pageId;
	BufType getDataReadonly() const;
	BufType getDataMutable() const;
	~Page();
};

#endif //TEST_PAGE_H
