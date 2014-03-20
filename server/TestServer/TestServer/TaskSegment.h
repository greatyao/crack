#pragma once

class CTaskSegment
{
public:
	CTaskSegment(void);
	~CTaskSegment(void);
	

	virtual INT GetSegmentNum(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon);
	virtual INT TaskSegment(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon);
};
