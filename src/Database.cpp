//
// Created by 王润基 on 2017/10/22.
//

#include <string>
#include <exception>
#include "Table.h"
#include "Database.h"
#include "DatabaseManager.h"

int Database::acquireNewPage() {
	BufType firstPageBuffer;
	int firstPageIndex;
	firstPageBuffer=this->databaseManager.bufPageManager->getPage(fileID,0,firstPageIndex);
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
	this->databaseManager.bufPageManager->markDirty(firstPageIndex);
	return pageID;
}

void Database::releasePage(int pageID){
	BufType firstPageBuffer;
	int firstPageIndex;
	firstPageBuffer=this->databaseManager.bufPageManager->getPage(fileID,0,firstPageIndex);
	unsigned char* pageMap=(unsigned char*)(firstPageBuffer+1024);
	int pos=pageID/8;
	int offset=pageID%8;
	if(((pageMap[pos]<<offset)&0x80)==0){
		throw ::std::runtime_error("Double page release");
	}
	pageMap[pos]&=~(1<<(7-offset));
	this->databaseManager.bufPageManager->markDirty(firstPageIndex);
}

Database::~Database() {
	this->databaseManager.bufPageManager->close();
	this->databaseManager.fileManager->closeFile(this->fileID);
}

void Database::createTable(::std::string name, size_t recordLength) {

}

void Database::deleteTable(Table *table) {

}

Table *Database::getTable(::std::string name) {
	return nullptr;
}
