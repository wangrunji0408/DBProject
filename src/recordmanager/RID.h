#ifndef RID_H
#define RID_H

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

	RID(unsigned short pageId, unsigned short slotId) :
			pageId(pageId), slotId(slotId) {}
};

#endif //RID_H