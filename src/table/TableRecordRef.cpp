//
// Created by 王润基 on 2018/1/1.
//

#include "TableRecordRef.h"

TableRecordRef::TableRecordRef(TableMetaPage *meta, const void *pData) :
		meta(meta), pData((const unsigned char*)pData) {}

std::bitset<128>* TableRecordRef::getNullBitsetPtr() const {
	return (std::bitset<128>*)(static_cast<const void*>(
									   pData + meta->recordLength - (meta->columnSize + 7) / 8));
}

bool TableRecordRef::isNullAtCol(int i) const {
	return getNullBitsetPtr()->test(i);
}

Data TableRecordRef::getDataAtCol(int i) const {
	auto const& col = meta->columns[i];
	auto begin = pData + col.offset;
	if(col.dataType == VARCHAR)
		return Data(begin, begin + strlen((char*)begin));
	return Data(begin, begin + col.size);
}
