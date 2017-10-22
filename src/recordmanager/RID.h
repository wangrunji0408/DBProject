#ifndef RID_H
#define RID_H

struct RID{
	unsigned int pageId;
	unsigned int slotId;

	bool operator==(const RID &rhs) const {
		return pageId == rhs.pageId &&
			   slotId == rhs.slotId;
	}

	bool operator!=(const RID &rhs) const {
		return !(rhs == *this);
	}
};

#endif //RID_H