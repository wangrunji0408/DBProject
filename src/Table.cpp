//
// Created by 王润基 on 2017/10/22.
//

#include <recordmanager/Record.h>
#include "Table.h"

bool any_filter(const Record& record)
{
	return true;
}

void Table::deleteData(){

}

size_t Table::getRecordLength() const {
	return 0;
}

Record Table::getRecord(RID const&) {
	return Record();
}

RID Table::insertRecord(BufType data) {
	return RID();
}

void Table::deleteRecord(RID const&) {

}

void Table::updateRecord(Record const&) {

}

RecordScanner Table::iterateRecords(::std::function<bool(const Record &)> filter) {
	return RecordScanner();
}
