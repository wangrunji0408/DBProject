//
// Created by 王润基 on 2017/10/24.
//

#include "Page.h"

BufType Page::getDataReadonly() const
{
	int bufIndex;
	auto bufData = bufPageManager->getPage(fileId, pageId, bufIndex);
	bufPageManager->access(bufIndex);
	return bufData;
}

BufType Page::getDataMutable() const
{
	int bufIndex;
	auto bufData = bufPageManager->getPage(fileId, pageId, bufIndex);
	bufPageManager->markDirty(bufIndex);
	return bufData;
}

Page::~Page()
{
}

Page::Page(BufPageManager *bufPageManager, int fileId, int pageId):
	bufPageManager(bufPageManager), fileId(fileId), pageId(pageId)
{
}

bool operator==(const Page &lhs, const Page &rhs) {
	return lhs.fileId == rhs.fileId &&
		   lhs.pageId == rhs.pageId;
}

bool operator!=(const Page &lhs, const Page &rhs) {
	return !(rhs == lhs);
}

Page& Page::operator=(Page const& rhs) {
	*(int*)&fileId = rhs.fileId;
	*(int*)&pageId = rhs.pageId;
	return *this;
}