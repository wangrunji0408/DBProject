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
	while(true){
		if(fieldId>=table->maxRecordPerPage){
			currentPageId=nextPageId;
			if(currentPageId<0){
				end=true;
				return;
			}
			Page currentPage = this->table->recordManager.database.getPage(currentPageId);
			BufType currentPageBuffer = currentPage.getDataReadonly();
			nextPageId=currentPageBuffer[1];
			fieldId=0;
		}else{
			fieldId++;
		}
		Page currentPage = this->table->recordManager.database.getPage(currentPageId);
		BufType currentPageBuffer = currentPage.getDataReadonly();
		unsigned char* recordMap=(unsigned char*)(currentPageBuffer)+8191;
		if(((recordMap[-(fieldId/8)]>>(fieldId%8))&0x1)!=0){
			break;
		}
	}
}

RecordScanner::~RecordScanner() {

}

RecordScanner::RecordScanner(Table* table):table(table){
	currentPageId=table->tablePageID;
	nextPageId=table->firstDataPageID;
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
	return table->getRecord({(unsigned int)currentPageId,(unsigned int)fieldId});
}

bool RecordScanner::hasNext() {
	if(needUpdate){
		update();
	}
	return !end;
}
