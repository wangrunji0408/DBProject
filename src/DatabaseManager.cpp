#include <string>
#include "DatabaseManager.h"

void DatabaseManager::createDatabase(::std::string name){
	::std::string filename=name+".dbf";
	this->fileManager->createFile(filename.c_str());
	int fileID;
	this->fileManager->openFile(filename.c_str(),fileID);
	unsigned int* firstPageBuffer=new unsigned int[2048];
	this->fileManager->readPage(fileID,0,firstPageBuffer,0);
	char* firstPageCharBuffer=((char*)firstPageBuffer);
	firstPageBuffer[63]=0;
	firstPageCharBuffer[0]='D';
	firstPageCharBuffer[1]='T';
	firstPageCharBuffer[2]='B';
	firstPageCharBuffer[3]='S';
	this->fileManager->writePage(fileID,0,firstPageBuffer,0);
	delete[] firstPageBuffer;
	this->fileManager->closeFile(fileID);
}