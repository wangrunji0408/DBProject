#include <iostream>
#include <string>
#include "DatabaseManager.h"

int main(int argc,char** argv){
	DatabaseManager dbm;
	dbm.createDatabase("test"s);
	return 0;
}