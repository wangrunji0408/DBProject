//
// Created by 王润基 on 2017/12/17.
//

#include "TableRecord.h"
#include <bitset>

TableRecord::TableRecord(TableMetaPage *meta, const void *pData) :
		meta(meta), pData((const unsigned char*)pData) {}

std::bitset<128>* TableRecord::getNullBitsetPtr() const {
	return (std::bitset<128>*)(static_cast<const void*>(
			pData + meta->recordLength - (meta->columnSize + 7) / 8));
}

bool TableRecord::isNullAtCol(int i) const {
	return getNullBitsetPtr()->test(i);
}

Data TableRecord::getData() const {
	return Data(pData, pData + meta->recordLength);
}

Data TableRecord::getDataAtCol(int i) const {
	auto const& col = meta->columns[i];
	return Data(pData + col.offset, pData + col.offset + col.size);
}

TableRecord::TableRecord(TableMetaPage *meta, RecordValue const &value) {
	if(value.values.size() != meta->columnSize)
		throw std::runtime_error("Value attr size not equal to column size");
	auto p = new unsigned char[meta->recordLength];
	this->meta = meta;
	pData = p;
	ownData = true;
	auto nullBitsetPtr = getNullBitsetPtr();
	for(int i=0; i<meta->columnSize; ++i) {
		auto& col = meta->columns[i];
		auto const& isNull = value.values[i].empty();
		nullBitsetPtr->set(static_cast<size_t>(i), isNull);
		if(!isNull)
			parse(value.values[i], p + col.offset, col.dataType, col.size);
	}
}

TableRecord::~TableRecord() {
	if(ownData)
		delete[] pData;
}

const unsigned char *TableRecord::getDataRef() const {
	return pData;
}

const unsigned char *TableRecord::getDataRefAtCol(int i) const {
	return pData + meta->columns[i].offset;
}
