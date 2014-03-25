#pragma once

#include <string>

class CTaskSegment
{
public:

	virtual INT GetSegmentNum(unsigned min_len,unsigned max_len,std::string &string_chars_set,unsigned split,const char *jhon);
	virtual INT TaskSegment(unsigned min_len,unsigned max_len,std::string &string_chars_set,unsigned split,const char *jhon);
};
