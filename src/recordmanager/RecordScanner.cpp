//
// Created by 王润基 on 2017/10/22.
//

#include "RID.h"
#include "Record.h"
#include "RecordScanner.h"
#include "Table.h"
#include "Database.h"
#include "DatabaseManager.h"
#include <iostream>
#include <exception>

void RecordScanner::update(){
	needUpdate=false;
	if(end){
		return;
	}
	BufType currentPageBuffer;
	int currentPageIndex;
	while(true){
		if(fieldId>=table->maxRecordPerPage){
			currentPage=nextPage;
			if(currentPage<0){
				end=true;
				return;
			}
			currentPageBuffer=this->table->database.databaseManager.bufPageManager->getPage(this->table->database.fileID,currentPage,currentPageIndex);
			nextPage=currentPageBuffer[1];
			this->table->database.databaseManager.bufPageManager->access(currentPageIndex);
			fieldId=0;
		}else{
			fieldId++;
		}
		currentPageBuffer=this->table->database.databaseManager.bufPageManager->getPage(this->table->database.fileID,currentPage,currentPageIndex);
		unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
		if(((recordMap[-(fieldId/8)]>>(fieldId%8))&0x1)!=0){
			break;
		}
		this->table->database.databaseManager.bufPageManager->access(currentPageIndex);
	}
}

RecordScanner::~RecordScanner() {

}

RecordScanner::RecordScanner(Table* table):table(table){
	currentPage=table->tablePageID;
	nextPage=table->firstDataPageID;
	fieldId=table->maxRecordPerPage;
}

Record RecordScanner::getNext() {
	if(end){
		throw ::std::runtime_error("There is no more record");
	}
	if(needUpdate){
		update();
	}
	needUpdate=true;
	return table->getRecord({(unsigned int)currentPage,(unsigned int)fieldId});
}

bool RecordScanner::hasNext() {
	if(needUpdate){
		update();
	}
	return !end;
}
