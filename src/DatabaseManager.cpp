#include <cstring>
#include <string>
#include <exception>
#include <memory>
#include <cstdio>
#include "DatabaseManager.h"
#include "filesystem/utils/pagedef.h"

void DatabaseManager::createDatabase(::std::string name){
	::std::string filename=name+".dbf";
	int created=this->fileManager->createFile(filename.c_str());
	if(created<0){
		throw ::std::runtime_error("Cannot create database file");
	}
	if(created>0){
		throw ::std::runtime_error("File existed");
	}
	int fileID;
	int opened=this->fileManager->openFile(filename.c_str(),fileID);
	if(opened!=0){
		throw ::std::runtime_error("Cannot open database file");
	}
	unsigned int* firstPageBuffer=new unsigned int[2048];
	this->fileManager->readPage(fileID,0,firstPageBuffer,0);
	::std::memset(firstPageBuffer,0,8192);
	unsigned char* firstPageCharBuffer=((unsigned char*)firstPageBuffer);
	firstPageBuffer[63]=0;
	firstPageCharBuffer[0]='D';
	firstPageCharBuffer[1]='T';
	firstPageCharBuffer[2]='B';
	firstPageCharBuffer[3]='S';
	firstPageCharBuffer[4096]=1<<7;
	this->fileManager->writePage(fileID,0,firstPageBuffer,0);
	delete[] firstPageBuffer;
	this->fileManager->closeFile(fileID);
}

void DatabaseManager::useDatabase(::std::string name) {
	::std::string filename=name+".dbf";
	int fileID;
	int opened=this->fileManager->openFile(filename.c_str(),fileID);
	if(opened!=0){
		throw ::std::runtime_error("Cannot open database file");
	}
	int pageIndex;
	BufType firstPageBuffer=this->bufPageManager->getPage(fileID,0,pageIndex);
	unsigned char* firstPageCharBuffer=((unsigned char*)firstPageBuffer);
	if(firstPageCharBuffer[0]!='D'||firstPageCharBuffer[1]!='T'||firstPageCharBuffer[2]!='B'||firstPageCharBuffer[3]!='S'){
		this->fileManager->closeFile(fileID);
		throw ::std::runtime_error("The file required is not a database");
	}
	bpm->access(pageIndex);
	this->currentDatabase.reset(new Database(*this,fileID,name));
}

Database *DatabaseManager::getCurrentDatabase() {
	if(this->currentDatabase==nullptr){
		throw ::std::runtime_error("There is no current database");
	}
	return this->currentDatabase.get();
}

void DatabaseManager::deleteCurrentDatabase() {
	::std::string filename=this->currentDatabase->name+".dbf";
	this->currentDatabase.reset(nullptr);
	::std::remove(filename.c_str());
}
