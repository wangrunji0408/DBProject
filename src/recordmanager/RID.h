#ifndef RID_H
#define RID_H

struct RID{
	const unsigned int pageId;
	const unsigned int slotId;

	bool operator==(const RID &rhs) const {
		return pageId == rhs.pageId &&
			   slotId == rhs.slotId;
	}

	bool operator!=(const RID &rhs) const {
		return !(rhs == *this);
	}

	RID operator= (RID const& rid)
	{
		return RID(rid.pageId, rid.slotId);
	}

	RID(unsigned int pageId, unsigned int slotId) :
			pageId(pageId), slotId(slotId) {}
};

#endif //RID_H