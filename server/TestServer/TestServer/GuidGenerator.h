//最后编辑 2014年1月17日

#ifndef __GUID_GEN__H__
#define __GUID_GEN__H__

#include <Objbase.h>
#include <string>


void gen_guid(unsigned char *pguid);

//创建一个guid字符串
std::string new_guid(void);

//GUID->string
void guid_to_string(GUID *,std::string &);

//string->GUID
void string_to_guid(std::string &,GUID *);

#endif