//
// Created by 王润基 on 2017/10/23.
//

#ifndef TABLEDEF_H
#define TABLEDEF_H

#include <string>
#include <vector>
#include <ostream>
#include "DataType.h"

struct ColumnDef {
	std::string name;
	DataType dataType;
	size_t size;
	bool nullable;
	bool unique;

	friend bool operator==(const ColumnDef &lhs, const ColumnDef &rhs) {
		return lhs.name == rhs.name &&
			   lhs.dataType == rhs.dataType &&
			   lhs.size == rhs.size &&
			   lhs.nullable == rhs.nullable &&
			   lhs.unique == rhs.unique;
	}

	friend bool operator!=(const ColumnDef &lhs, const ColumnDef &rhs) {
		return !(rhs == lhs);
	}

	friend ::std::ostream& operator<<(::std::ostream& out,const ColumnDef cd){
		out<<cd.name<<" ";
		switch(cd.dataType){
		case INT:
			out<<"int("<<cd.size<<")";
			break;
		case FLOAT:
			out<<"float";
			break;
		case DATE:
			out<<"date";
			break;
		case VARCHAR:
			out<<"varchar("<<cd.size<<")";
			break;
		default:
			out<<"unknown";
			break;
		}
		if(cd.unique){
			out<<" unique";
		}
		if(!cd.nullable){
			out<<" not null";
		}
		return out;
	}
};

struct ForeignKeyDef {
	std::string keyName;
	std::string refTable;
	std::string refName;

	friend bool operator==(const ForeignKeyDef &lhs, const ForeignKeyDef &rhs) {
		return lhs.keyName == rhs.keyName &&
			   lhs.refTable == rhs.refTable &&
			   lhs.refName == rhs.refName;
	}

	friend bool operator!=(const ForeignKeyDef &lhs, const ForeignKeyDef &rhs) {
		return !(rhs == lhs);
	}

	friend ::std::ostream& operator<<(::std::ostream& out,const ForeignKeyDef fkd){
		out<<"foreign key("<<fkd.keyName<<")references "<<fkd.refTable<<"("<<fkd.refName<<")";
		return out;
	}
};

struct TableDef {
	std::string name;
	std::vector<ColumnDef> columns;
	std::vector<ForeignKeyDef> foreignKeys;
	std::vector<std::string> primaryKeys;

	friend bool operator==(const TableDef &lhs, const TableDef &rhs) {
		return lhs.name == rhs.name &&
			   lhs.columns == rhs.columns &&
			   lhs.foreignKeys == rhs.foreignKeys &&
			   lhs.primaryKeys == rhs.primaryKeys;
	}

	friend bool operator!=(const TableDef &lhs, const TableDef &rhs) {
		return !(rhs == lhs);
	}

	friend ::std::ostream& operator<<(::std::ostream& out,const TableDef td){
		out<<"table "<<td.name<<"(\n";
		bool first=true;
		for(ColumnDef cd:td.columns){
			if(!first){
				out<<",\n";
			}
			out<<"\t"<<cd;
			first=false;
		}
		if(td.primaryKeys.size()>0){
			if(!first){
				out<<",\n";
			}
			first=false;
			out<<"\tprimary key(";
			bool firstPrim=true;
			for(::std::string prim:td.primaryKeys){
				if(!firstPrim){
					out<<",";
				}
				out<<prim;
				firstPrim=false;
			}
			out<<")";
		}
		for(ForeignKeyDef fkd:td.foreignKeys){
			if(!first){
				out<<",\n";
			}
			out<<"\t"<<fkd;
			first=false;
		}
		out<<"\n)\n";
		return out;
	}
};

#endif //TABLEDEF_H
