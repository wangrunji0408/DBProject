#include <iostream>
#include <string>
#include "DatabaseManager.h"

int main(int argc,char** argv){
	DatabaseManager dbm;
	dbm.createDatabase("test"s);
	dbm.useDatabase("test"s);
	// dbm.deleteCurrentDatabase();
	return 0;
}