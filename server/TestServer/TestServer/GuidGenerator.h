//���༭ 2014��1��17��

#ifndef __GUID_GEN__H__
#define __GUID_GEN__H__

#include <Objbase.h>
#include <string>


void gen_guid(unsigned char *pguid);

//����һ��guid�ַ���
std::string new_guid(void);

//GUID->string
void guid_to_string(GUID *,std::string &);

//string->GUID
void string_to_guid(std::string &,GUID *);

#endif