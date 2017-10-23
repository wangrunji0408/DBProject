//
// Created by 王润基 on 2017/10/22.
//

#include <recordmanager/Record.h>
#include <cstring>
#include <exception>
#include "Table.h"
#include "Database.h"
#include "DatabaseManager.h"

bool any_filter(const Record& record)
{
	return true;
}

void Table::deleteData(){
	BufType currentPageBuffer;
	int currentPageIndex;
	int lastPageID=tablePageID;
	int currentPageID=firstDataPageID;
	while(currentPageID>=0){
		tablePageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,tablePageID,tablePageIndex);
		lastPageID=currentPageID;
		currentPageID=currentPageBuffer[1];
		this->database.databaseManager.bufPageManager->access(tablePageIndex);
		this->database.releasePage(lastPageID);
	}
}


void Table::recoverMetadata() {
	BufType tablePageBuffer;
	int tablePageIndex;
	tablePageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,tablePageID,tablePageIndex);
	recordLength=tablePageBuffer[63];
	firstDataPageID=tablePageBuffer[62];
	maxRecordPerPage=(8*8096)/(8*recordLength+1);
	this->database.databaseManager.bufPageManager->access(tablePageIndex);
}

size_t Table::getRecordLength() const {
	return recordLength;
}

Record Table::getRecord(RID const& rid) {
	int pageID=rid.pageId;
	int recordID=rid.slotId;
	BufType currentPageBuffer;
	int currentPageIndex;
	currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,pageID,currentPageIndex);
	if(currentPageBuffer[2]!=tablePageID){
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("This is not RID for this table");
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	if(((recordMap[-(recordID/8)]>>(recordID%8))&0x1)==0){
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("There is not a record for this RID");
	}
	unsigned char* source=(unsigned char*)(currentPageBuffer+24)+recordID*recordLength;
	unsigned char* data=new unsigned char[recordLength];
	::std::memcpy(data,source,recordLength);
	this->database.databaseManager.bufPageManager->access(currentPageIndex);
	return {rid,data};
}

RID Table::insertRecord(unsigned char* data) {
	BufType currentPageBuffer;
	int currentPageIndex;
	int lastPageID=tablePageID;
	int currentPageID=firstDataPageID;
	while(currentPageID>=0){
		currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,currentPageID,currentPageIndex);
		if(currentPageBuffer[23]<maxRecordPerPage){
			break;
		}
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
	}
	if(currentPageID<0){
		currentPageID=this->database.acquireNewPage();
		currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,lastPageID,currentPageIndex);
		if(lastPageID==tablePageID){
			currentPageBuffer[62]=currentPageID;
		}else{
			currentPageBuffer[1]=currentPageID;
		}
		this->database.databaseManager.bufPageManager->markDirty(currentPageIndex);
		currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,currentPageID,currentPageIndex);
		::std::memset(currentPageBuffer,0,8192);
		currentPageBuffer[0]=(lastPageID==tablePageID)?-1:lastPageID;
		currentPageBuffer[1]=-1;
		currentPageBuffer[2]=tablePageID;
		currentPageBuffer[22]=recordLength;
	}else{
		currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,currentPageID,currentPageIndex);
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	unsigned int recordID=0;
	int mapOffset=0;
	while(recordID<maxRecordPerPage){
		if(recordMap[mapOffset]!=0xFF){
			break;
		}
		mapOffset--;
		recordID+=8;
	}
	unsigned char mapDetail=recordMap[mapOffset];
	while((mapDetail&0x1)!=0){
		recordID++;
		mapDetail>>1;
	}
	recordMap[mapOffset]|=(1<<(recordID%8));
	unsigned char* dest=(unsigned char*)(currentPageBuffer+24)+recordID*recordLength;
	::std::memcpy(dest,data,recordLength);
	currentPageBuffer[23]++;
	this->database.databaseManager.bufPageManager->markDirty(currentPageIndex);
	return {(unsigned int)currentPageID,recordID};
}

void Table::deleteRecord(RID const& rid) {
	int pageID=rid.pageId;
	int recordID=rid.slotId;
	BufType currentPageBuffer;
	int currentPageIndex;
	currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,pageID,currentPageIndex);
	if(currentPageBuffer[2]!=tablePageID){
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("This is not RID for this table");
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	if(((recordMap[-(recordID/8)]>>(recordID%8))&0x1)==0){
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("Double delete of record");
	}
	recordMap[-(recordID/8)]&=~(1<<(recordID%8));
	currentPageBuffer[23]--;
	int remain=currentPageBuffer[23];
	int lastPageID=currentPageBuffer[0];
	int nextPageID=currentPageBuffer[1];
	this->database.databaseManager.bufPageManager->markDirty(currentPageIndex);
	if(remain==0){
		this->database.releasePage(pageID);
		currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,(lastPageID==-1)?tablePageID:lastPageID,currentPageIndex);
		if(lastPageID==-1){
			currentPageBuffer[62]=nextPageID;
			firstDataPageID=nextPageID;
		}else{
			currentPageBuffer[1]=nextPageID;
		}
		this->database.databaseManager.bufPageManager->markDirty(currentPageIndex);
		if(nextPageID!=-1){
			currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,(lastPageID==-1)?tablePageID:nextPageID,currentPageIndex);
			currentPageBuffer[0]=lastPageID;
			this->database.databaseManager.bufPageManager->markDirty(currentPageIndex);
		}
	}
}

void Table::updateRecord(Record const& record) {
	RID rid=record.recordID;
	int pageID=rid.pageId;
	int recordID=rid.slotId;
	BufType currentPageBuffer;
	int currentPageIndex;
	currentPageBuffer=this->database.databaseManager.bufPageManager->getPage(this->database.fileID,pageID,currentPageIndex);
	if(currentPageBuffer[2]!=tablePageID){
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("This is not RID for this table");
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	if(((recordMap[-(recordID/8)]>>(recordID%8))&0x1)==0){
		this->database.databaseManager.bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("There is not a record for this RID");
	}
	unsigned char* dest=(unsigned char*)(currentPageBuffer+24)+recordID*recordLength;
	::std::memcpy(dest,record.data,recordLength);
	this->database.databaseManager.bufPageManager->markDirty(currentPageIndex);
}

RecordScanner Table::iterateRecords(::std::function<bool(const Record &)> filter) {
	return RecordScanner();
}
