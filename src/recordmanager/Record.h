#ifndef RECORD_H
#define RECORD_H

#include "filesystem/utils/pagedef.h"
#include "RID.h"

struct Record{
	const RID recordID;
	BufType data;
};

#endif //RECORD_H