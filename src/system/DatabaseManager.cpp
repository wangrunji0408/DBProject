#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <cstring>
#include <string>
#include <index/SysIndexPage.h>
#include "DatabaseManager.h"
#include "DatabaseMetaPage.h"

bool DatabaseManager::hasInstance = false;

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
	auto firstPage = DatabaseMetaPage();
	auto sysIndexPage = SysIndexPage();
	this->fileManager->writePage(fileID,0,(BufType)&firstPage,0);
	this->fileManager->writePage(fileID,1,(BufType)&sysIndexPage,0);
	this->fileManager->closeFile(fileID);
}

void DatabaseManager::useDatabase(::std::string name) {
	this->currentDatabase.reset(nullptr);
	::std::string filename=name+".dbf";
	int fileID;
	int opened=this->fileManager->openFile(filename.c_str(),fileID);
	if(opened!=0){
		throw ::std::runtime_error("Cannot open database file");
	}
	int pageIndex;
	BufType firstPageBuffer=this->bufPageManager->getPage(fileID,0,pageIndex);
	try {((DatabaseMetaPage*)firstPageBuffer)->check();}
	catch (std::exception const& e)
	{
		this->bufPageManager->close();
		this->fileManager->closeFile(fileID);
		throw ::std::runtime_error("The file required is not a database");
	}
	this->bufPageManager->access(pageIndex);
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

::std::vector<::std::string> DatabaseManager::getDatabases(){
	//sorry, we use something from posix&mingw-w64 to read folder
	DIR* currentDir=opendir(".");
	if(currentDir==nullptr){
		throw ::std::runtime_error("Cannot open current folder");
	}
	::std::vector<::std::string> content;
	while(true){
		errno=0;
		dirent* entry=readdir(currentDir);
		//sorry for not check entry type here
		if(entry==nullptr){
			if(errno!=0){
				throw ::std::runtime_error("Error when reading folder content");
			}else{
				if(closedir(currentDir)<0){
					throw ::std::runtime_error("Error when reading folder content");
				}
				return content;
			}
		}else{
			::std::string name(entry->d_name);
			if(name.length()>=4){
				if(name.substr(name.length()-4)==".dbf"){
					content.push_back(name.substr(0,name.length()-4));
				}
			}
		}
	}
}