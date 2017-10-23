#include <iostream>
#include <string>
#include "Table.h"
#include "Database.h"
#include "DatabaseManager.h"

int main(int argc,char** argv){
	DatabaseManager dbm;
	dbm.createDatabase("test"s);
	dbm.useDatabase("test"s);
	Database* db=dbm.getCurrentDatabase();
	db->createTable("table"s,10);
	Table* t=db->getTable("table"s);
	t->insertRecord((unsigned char*)"benpigchu");
	// dbm.deleteCurrentDatabase();
	return 0;
}