//
// Created by 王润基 on 2017/10/22.
//

#include <string>
#include <cstring>
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

void Database::recoverTables() {
	BufType firstPageBuffer;
	int firstPageIndex;
	firstPageBuffer=this->databaseManager.bufPageManager->getPage(fileID,0,firstPageIndex);
	tableCount=firstPageBuffer[63];
	for(int i=0;i<tableCount;i++){
		BufType tableRecordPos=firstPageBuffer+64+i*32;
		int tablePageID=*tableRecordPos;
		unsigned char* tableNamePos=(unsigned char*)(tableRecordPos+1);
		unsigned char tableName[125];
		::std::memcpy(tableName,tableNamePos,124);
		tableName[125]='\0';
		::std::string name((char*)(tableName));
		tables[i].reset(new Table(*this,name,tablePageID));
	}
	this->databaseManager.bufPageManager->access(firstPageIndex);
}

void Database::createTable(::std::string name, size_t recordLength) {
	if(recordLength<=0){
		throw ::std::runtime_error("The length of record is not positive");
	}
	if(recordLength>=8096){
		throw ::std::runtime_error("The length of record is too large");
	}
	if(tableCount>=30){
		throw ::std::runtime_error("Table count exceeded");
	}
	if(name.length()>124){
		throw ::std::runtime_error("Table name too long");
	}
	for(int i=0;i<tableCount;i++){
		if(tables[i]->name==name){
			throw ::std::runtime_error("A table with this name is already exist");
		}
	}
	int tablePageID=acquireNewPage();
	BufType tablePageBuffer;
	int tablePageIndex;
	tablePageBuffer=this->databaseManager.bufPageManager->getPage(fileID,tablePageID,tablePageIndex);
	::std::memset(tablePageBuffer,0,8192);
	tablePageBuffer[62]=(unsigned int)(recordLength);
	tablePageBuffer[61]=-1;
	this->databaseManager.bufPageManager->markDirty(tablePageIndex);
	BufType firstPageBuffer;
	int firstPageIndex;
	firstPageBuffer=this->databaseManager.bufPageManager->getPage(fileID,0,firstPageIndex);
	int tableIndex=firstPageBuffer[63];
	firstPageBuffer[63]++;
	tableCount++;
	BufType tableRecordPos=firstPageBuffer+64+tableIndex*32;
	*tableRecordPos=tablePageID;
	unsigned char* tableNamePos=(unsigned char*)(tableRecordPos+1);
	::std::strncpy((char*)tableNamePos,name.c_str(),124);
	this->databaseManager.bufPageManager->markDirty(firstPageIndex);
	tables[tableIndex].reset(new Table(*this,name,tablePageID));
}

void Database::deleteTable(Table *table) {
	for(int i=0;i<tableCount;i++){
		if(tables[i].get()==table){
			releasePage(tables[i]->tablePageID);
			tables[i]->deleteData();
			BufType firstPageBuffer;
			int firstPageIndex;
			firstPageBuffer=this->databaseManager.bufPageManager->getPage(fileID,0,firstPageIndex);
			tableCount--;
			firstPageBuffer[63]--;
			if(i!=tableCount){
				unsigned char* oldTablePos=(unsigned char*)(firstPageBuffer+64+i*32);
				unsigned char* lastTablePos=(unsigned char*)(firstPageBuffer+64+tableCount*32);
				::std::memcpy(oldTablePos,lastTablePos,128);
				tables[i].swap(tables[tableCount]);
			}
			tables[tableCount].reset(nullptr);
			this->databaseManager.bufPageManager->markDirty(firstPageIndex);
			return;
		}
	}
	// throw ::std::runtime_error("This table pointer do not appear in the database");
}

Table *Database::getTable(::std::string name) {
	for(int i=0;i<tableCount;i++){
		if(tables[i]->name==name){
			return tables[i].get();
		}
	}
	return nullptr;
	// throw ::std::runtime_error("A table with this name do not exist");
}
