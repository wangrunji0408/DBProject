//
// Created by 王润基 on 2017/10/22.
//

#include <recordmanager/Record.h>
#include "Table.h"

unsigned long Table::getRecordLength() const {
	return 0;
}

Record Table::getRecord(const RID) {
	return Record();
}

const RID Table::insertRecord(BufType data) {
	return RID();
}

void Table::deleteRecord(const RID) {

}

void Table::updateRecord(const Record) {

}

RecordScanner Table::iterateRecords(::std::function<const Record(bool)> filter) {
	return RecordScanner();
}
