//
// Created by 王润基 on 2017/10/24.
//

#include "Page.h"

BufType Page::getDataReadonly()
{
	loadToBuf();
	bufPageManager->access(bufIndex);
	return bufData;
}

BufType Page::getDataMutable()
{
	loadToBuf();
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

void Page::loadToBuf() {
	bufData = bufPageManager->getPage(fileId, pageId, bufIndex);
}
