//
// Created by 王润基 on 2017/10/22.
//

#include "Record.h"
#include "RecordScanner.h"

RecordScanner::~RecordScanner() {

}

RecordScanner::RecordScanner() {

}

Record RecordScanner::getNext() {
	return Record();
}

bool RecordScanner::hasNext() {
	return false;
}
