//
// Created by 王润基 on 2017/11/7.
//

#ifndef TEST_TABLEMETAPAGE_H
#define TEST_TABLEMETAPAGE_H


#include <ast/TableDef.h>

class RecordManager;

struct TableMetaPage {

	static const int MAX_COLUMN_NAME_LENGTH = 43;
	static const int MAX_COLUMN_SIZE = 124;
	static const int MAX_NAME_LENGTH = 124;
	static const int MAX_RECORD_LENGTH = 8096;

	struct Column { // size = 64
		short size;
		short offset;
		DataType dataType;	// char
		char name[MAX_COLUMN_NAME_LENGTH];
		short indexID;
		short foreignTableID;
		short foreignColumnID;
		char primaryKey: 1;
		char nullable: 1;
		char unique: 1;
		char reserved[4];

		void makeFromDef(ColumnDef const& def);
		ColumnDef toDef() const;
	};

	char reserved[120];
	char name[MAX_NAME_LENGTH+1];
	short columnSize;
	int firstPageID;	// -1 if not exist
	int recordLength;
	Column columns[MAX_COLUMN_SIZE];

	int getColomnId(std::string name) const;
	void makeFromDef(TableDef const& def, RecordManager& recordManager);
	TableDef toDef(RecordManager& recordManager) const;
};


#endif //TEST_TABLEMETAPAGE_H
