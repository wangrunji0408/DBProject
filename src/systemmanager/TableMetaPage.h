//
// Created by 王润基 on 2017/11/7.
//

#ifndef TEST_TABLEMETAPAGE_H
#define TEST_TABLEMETAPAGE_H


#include <ast/TableDef.h>

struct TableMetaPage {

	static const int MAX_COLUMN_NAME_LENGTH = 43;
	static const int MAX_COLUMN_SIZE = 100;

	struct Column { // size = 64
		short size;
		short offset;
		DataType dataType;	// char
		char name[MAX_COLUMN_NAME_LENGTH];
		short indexID;
		short foreignTableID;
		short foreignColumnID;
		char primary: 1;
		char nullable: 1;
		char unique: 1;
	};

	char reserved[248];
	int firstPageID;	// -1 if not exist
	int recordLength;
	Column columns[MAX_COLUMN_SIZE];

};


#endif //TEST_TABLEMETAPAGE_H
