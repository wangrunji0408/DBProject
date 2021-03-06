//
// Created by 王润基 on 2017/10/22.
//

#include <record/Record.h>
#include "RecordSet.h"
#include "system/Database.h"
#include "system/DatabaseManager.h"

RecordSet::RecordSet(RecordManager& recordManager,::std::string name,int tablePageID,int id):
	recordManager(recordManager), name(name), tablePageID(tablePageID), id(id),
	database(recordManager.database), bufPageManager(recordManager.database.databaseManager.bufPageManager.get())
{
	recoverMetadata();
}

void RecordSet::deleteData(){
	int lastPageID=tablePageID;
	int currentPageID=firstDataPageID;
	while(currentPageID>=0){
		auto currentPage = database.getPage(currentPageID);
		BufType currentPageBuffer = currentPage.getDataReadonly();
		lastPageID=currentPageID;
		currentPageID=currentPageBuffer[1];
		database.releasePage(lastPageID);
	}
}


void RecordSet::recoverMetadata() {
	auto metaPage = database.getPage(tablePageID);
	auto meta = (TableMetaPage*)metaPage.getDataReadonly();
	recordLength = static_cast<size_t>(meta->recordLength);
	firstDataPageID = meta->firstPageID;
	maxRecordPerPage=(8*8096)/(8*recordLength+1);
}

size_t RecordSet::getRecordLength() const {
	return recordLength;
}

Record RecordSet::getRecord(RID const& rid) {
	int pageID=rid.pageId;
	int recordID=rid.slotId;
	auto currentPage = database.getPage(pageID);
	BufType currentPageBuffer = currentPage.getDataReadonly();
	if(currentPageBuffer[2]!=tablePageID){
		throw ::std::runtime_error("This is not RID for this table");
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	if(((recordMap[-(recordID/8)]>>(recordID%8))&0x1)==0){
		throw ::std::runtime_error("There is not a record for this RID");
	}
	unsigned char* source=(unsigned char*)(currentPageBuffer+24)+recordID*recordLength;
	return {rid,source};
}

RID RecordSet::insert(const uchar *data) {
	BufType currentPageBuffer;
	int currentPageIndex;
	int lastPageID=tablePageID;
	int currentPageID=firstDataPageID;
	while(currentPageID>=0){
		auto currentPage = database.getPage(currentPageID);
		currentPageBuffer = currentPage.getDataReadonly();
		if(currentPageBuffer[23]<maxRecordPerPage){
			break;
		}
		lastPageID=currentPageID;
		currentPageID=currentPageBuffer[1];
	}
	if(currentPageID<0){
		currentPageID=database.acquireNewPage().pageId;
		currentPageBuffer=bufPageManager->getPage(this->database.fileID,lastPageID,currentPageIndex);
		if(lastPageID==tablePageID){
			currentPageBuffer[62]=currentPageID;
			firstDataPageID=currentPageID;
		}else{
			currentPageBuffer[1]=currentPageID;
		}
		bufPageManager->markDirty(currentPageIndex);
		currentPageBuffer=bufPageManager->getPage(this->database.fileID,currentPageID,currentPageIndex);
		::std::memset(currentPageBuffer,0,8192);
		currentPageBuffer[0]=(lastPageID==tablePageID)?-1:lastPageID;
		currentPageBuffer[1]=-1;
		currentPageBuffer[2]=tablePageID;
		currentPageBuffer[22]=recordLength;
	}else{
		currentPageBuffer=bufPageManager->getPage(this->database.fileID,currentPageID,currentPageIndex);
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
		mapDetail>>=1;
	}
	recordMap[mapOffset]|=(1<<(recordID%8));
	unsigned char* dest=(unsigned char*)(currentPageBuffer+24)+recordID*recordLength;
	::std::memcpy(dest,data,recordLength);
	currentPageBuffer[23]++;
	bufPageManager->markDirty(currentPageIndex);
	return RID(currentPageID,recordID);
}

void RecordSet::remove(RID const &rid) {
	int pageID=rid.pageId;
	int recordID=rid.slotId;
	BufType currentPageBuffer;
	int currentPageIndex;
	currentPageBuffer=bufPageManager->getPage(this->database.fileID,pageID,currentPageIndex);
	if(currentPageBuffer[2]!=tablePageID){
		bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("This is not RID for this table");
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	if(((recordMap[-(recordID/8)]>>(recordID%8))&0x1)==0){
		bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("Double delete of record");
	}
	recordMap[-(recordID/8)]&=~(1<<(recordID%8));
	currentPageBuffer[23]--;
	int remain=currentPageBuffer[23];
	int lastPageID=currentPageBuffer[0];
	int nextPageID=currentPageBuffer[1];
	bufPageManager->markDirty(currentPageIndex);
	if(remain==0){
		this->recordManager.database.releasePage(pageID);
		currentPageBuffer=bufPageManager->getPage(this->database.fileID,(lastPageID==-1)?tablePageID:lastPageID,currentPageIndex);
		if(lastPageID==-1){
			currentPageBuffer[62]=nextPageID;
			firstDataPageID=nextPageID;
		}else{
			currentPageBuffer[1]=nextPageID;
		}
		bufPageManager->markDirty(currentPageIndex);
		if(nextPageID!=-1){
			currentPageBuffer=bufPageManager->getPage(this->database.fileID,(lastPageID==-1)?tablePageID:nextPageID,currentPageIndex);
			currentPageBuffer[0]=lastPageID;
			bufPageManager->markDirty(currentPageIndex);
		}
	}
}

void RecordSet::update(Record const &record) {
	RID rid=record.recordID;
	int pageID=rid.pageId;
	int recordID=rid.slotId;
	BufType currentPageBuffer;
	int currentPageIndex;
	currentPageBuffer=bufPageManager->getPage(this->database.fileID,pageID,currentPageIndex);
	if(currentPageBuffer[2]!=tablePageID){
		bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("This is not RID for this table");
	}
	unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
	if(((recordMap[-(recordID/8)]>>(recordID%8))&0x1)==0){
		bufPageManager->access(currentPageIndex);
		throw ::std::runtime_error("There is not a record for this RID");
	}
	unsigned char* dest=(unsigned char*)(currentPageBuffer+24)+recordID*recordLength;
	::std::memcpy(dest,record.data,recordLength);
	bufPageManager->markDirty(currentPageIndex);
}

RecordScanner RecordSet::iterateRecords() {
	return RecordScanner(this);
}
