#ifndef RID_H
#define RID_H

#include <ostream>

struct RID{
	unsigned short pageId;
	unsigned short slotId;

	bool operator==(const RID &rhs) const {
		return pageId == rhs.pageId &&
			   slotId == rhs.slotId;
	}

	bool operator!=(const RID &rhs) const {
		return !(rhs == *this);
	}

	friend bool operator<(const RID &lhs, const RID &rhs) {
		if (lhs.pageId < rhs.pageId)
			return true;
		if (rhs.pageId < lhs.pageId)
			return false;
		return lhs.slotId < rhs.slotId;
	}

	friend bool operator>(const RID &lhs, const RID &rhs) {
		return rhs < lhs;
	}

	friend bool operator<=(const RID &lhs, const RID &rhs) {
		return !(rhs < lhs);
	}

	friend bool operator>=(const RID &lhs, const RID &rhs) {
		return !(lhs < rhs);
	}

	friend std::ostream &operator<<(std::ostream &os, const RID &rid) {
		os << "(" << rid.pageId << "," << rid.slotId << ")";
		return os;
	}

	RID(unsigned short pageId, unsigned short slotId) :
			pageId(pageId), slotId(slotId) {}
	RID() : RID(0,0) {}
};

#endif //RID_H