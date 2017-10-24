//
// Created by 王润基 on 2017/10/24.
//

#include "RecordManager.h"
#include "Database.h"
#include "DatabaseManager.h"

int RecordManager::acquireNewPage() {return database.acquireNewPage();}
void RecordManager::releasePage(int pageID) {database.releasePage(pageID);}


RecordManager::RecordManager(Database &database):
		database(database), databaseManager(database.databaseManager)
{
	// TODO temp
	fileID = database.fileID;

	recoverTables();
}

RecordManager::~RecordManager() {

}

void RecordManager::recoverTables() {
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

void RecordManager::createTable(::std::string name, size_t recordLength) {
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
	tablePageBuffer[63]=(unsigned int)(recordLength);
	tablePageBuffer[62]=-1;
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

void RecordManager::deleteTable(Table *table) {
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

Table *RecordManager::getTable(::std::string name) {
	for(int i=0;i<tableCount;i++){
		if(tables[i]->name==name){
			return tables[i].get();
		}
	}
	throw ::std::runtime_error("A table with this name do not exist");
}

