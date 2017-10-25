//
// Created by 王润基 on 2017/10/24.
//

#include "RecordManager.h"
#include "systemmanager/Database.h"

RecordManager::RecordManager(Database &database):
	database(database)
{
	recoverTables();
}

RecordManager::~RecordManager() {

}

void RecordManager::recoverTables() {
	auto firstPage = database.getPage(0);
	auto firstPageBuffer = (BufType)firstPage.getDataMutable();
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

	auto tablePage = database.acquireNewPage();
	BufType tablePageBuffer = (BufType)tablePage.getDataMutable();
	::std::memset(tablePageBuffer,0,8192);
	tablePageBuffer[63]=(unsigned int)(recordLength);
	tablePageBuffer[62]=-1;

	auto firstPage = database.getPage(0);
	BufType firstPageBuffer = (BufType)firstPage.getDataMutable();
	int tableIndex=firstPageBuffer[63];
	firstPageBuffer[63]++;
	tableCount++;
	BufType tableRecordPos=firstPageBuffer+64+tableIndex*32;
	*tableRecordPos=(uint)tablePage.pageId;
	unsigned char* tableNamePos=(unsigned char*)(tableRecordPos+1);
	::std::strncpy((char*)tableNamePos,name.c_str(),124);
	tables[tableIndex].reset(new Table(*this,name,tablePage.pageId));
}

void RecordManager::deleteTable(Table *table) {
	for(int i=0;i<tableCount;i++){
		if(tables[i].get()==table){
			database.releasePage(tables[i]->tablePageID);
			tables[i]->deleteData();
			auto firstPage = database.getPage(0);
			auto firstPageBuffer = (BufType)firstPage.getDataMutable();
			tableCount--;
			firstPageBuffer[63]--;
			if(i!=tableCount){
				unsigned char* oldTablePos=(unsigned char*)(firstPageBuffer+64+i*32);
				unsigned char* lastTablePos=(unsigned char*)(firstPageBuffer+64+tableCount*32);
				::std::memcpy(oldTablePos,lastTablePos,128);
				tables[i].swap(tables[tableCount]);
			}
			tables[tableCount].reset(nullptr);
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

