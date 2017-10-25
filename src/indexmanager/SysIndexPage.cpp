//
// Created by 王润基 on 2017/10/25.
//

#include <cstring>
#include <stdexcept>
#include "SysIndexPage.h"

void SysIndexPage::makeInit() {
	std::memset(this, 0, sizeof(SysIndexPage));
}

SysIndexPage::SysIndexPage() {
	makeInit();
}

int SysIndexPage::acquireNewIndex() {
	for(int i=0; i<MAX_INDEX_NUM; ++i)
		if(!indexInfo[i].used)
		{
			indexInfo[i].used = true;
			return i;
		}
	throw std::runtime_error("No more index available.");
}
