#pragma once
#include "tasksegment.h"

class CSimpleTaskSegment :
	public CTaskSegment
{
public:
	CSimpleTaskSegment(void);
	~CSimpleTaskSegment(void);

	INT GetSegmentNum(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon);
	INT TaskSegment(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon);

};
