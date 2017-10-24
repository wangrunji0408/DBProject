//
// Created by 王润基 on 2017/10/22.
//

#include <string>
#include <cstring>
#include <exception>
#include "Table.h"
#include "Database.h"
#include "DatabaseManager.h"

Page Database::acquireNewPage() {
	auto firstPage = getPage(0);
	BufType firstPageBuffer = firstPage.getDataMutable();
	unsigned char* pageMap=(unsigned char*)(firstPageBuffer+1024);
	int pageID=0;
	int i=0;
	for(;i<4096;i++){
		if(pageMap[i]!=0xFF){
			break;
		}
		pageID+=8;
	}
	if(pageID>=32768){
		throw ::std::runtime_error("No more page available");
	}
	unsigned char mapDetail=pageMap[i];
	while((mapDetail&0x80)!=0){
		pageID++;
		mapDetail<<=1;
	}
	pageMap[i]|=(1<<(7-(pageID%8)));
	return Page(this->databaseManager.bufPageManager.get(), fileID, pageID);
}

bool Database::isPageUsed(int pageID) const {
	if(pageID == 0)	return true;
	auto firstPage = getPage(0);
	BufType firstPageBuffer = firstPage.getDataReadonly();
	unsigned char* pageMap=(unsigned char*)(firstPageBuffer+1024);
	int pos=pageID/8;
	int offset=pageID%8;
	return (bool)((pageMap[pos]<<offset)&0x80);
}

void Database::releasePage(int pageID){
	auto firstPage = getPage(0);
	BufType firstPageBuffer = firstPage.getDataMutable();
	unsigned char* pageMap=(unsigned char*)(firstPageBuffer+1024);
	int pos=pageID/8;
	int offset=pageID%8;
	if(((pageMap[pos]<<offset)&0x80)==0){
		throw ::std::runtime_error("Double page release");
	}
	pageMap[pos]&=~(1<<(7-offset));
}

Page Database::getPage(int pageId) const {
	if(!isPageUsed(pageId))
		throw ::std::runtime_error("Try to get a unused page.");
	return Page(this->databaseManager.bufPageManager.get(), fileID, pageId);
}

Database::~Database() {
	this->databaseManager.bufPageManager->close();
	this->databaseManager.fileManager->closeFile(this->fileID);
}

void Database::createTable(::std::string name, size_t recordLength) {
	recordManager->createTable(name, recordLength);
}

void Database::deleteTable(Table *table) {
	recordManager->deleteTable(table);
}

Table *Database::getTable(::std::string name) {
	return recordManager->getTable(name);
}
