//
// Created by 王润基 on 2017/10/24.
//

#include "Page.h"

const uchar* Page::getDataReadonly() const
{
	bufPageManager->access(0);
	return bufData;
}

uchar* Page::getDataMutable()
{
	bufPageManager->access(0);
	dirty = true;
	return bufData;
}

Page::~Page()
{
	if(dirty)
		bufPageManager->markDirty(bufIndex);
}

Page::Page(BufPageManager *bufPageManager, int fileId, int pageId):
	fileId(fileId), pageId(pageId)
{
	this->bufPageManager = bufPageManager;
	bufPageManager->getPage(fileId, pageId, bufIndex);
}
