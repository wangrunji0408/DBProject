//
// Created by 王润基 on 2017/10/30.
//

#ifndef TEST_INDEXENTITYLISTS_H
#define TEST_INDEXENTITYLISTS_H

class Database;
class RecordSet;

/**
 * 管理相同键值链表群
 * 实际上是用一个数据表存储
 */
class IndexEntityLists {
	friend class IndexManager;

	static constexpr const char* ENTITYLIST_TABLE_NAME = "**IndexEntityList**";
	struct ELNode {
		RID dataRID, nextRID;
	};
	RecordSet* elTable;
	Database& database;
	IndexEntityLists(Database& database);
public:
	RID createList() const;
	void deleteList(RID const &headRID) const;
	bool isEmpty(RID const &headRID) const;
	bool containsEntity(RID const &dataRID, RID const &headRID) const;
	void insertEntity(RID const &dataRID, RID const &headRID) const;
	void removeEntity(RID const &dataRID, RID const &headRID) const;
};


#endif //TEST_INDEXENTITYLISTS_H
