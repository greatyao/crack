#include "stdafx.h"
#include "GuidGenerator.h"

#pragma comment(lib,"Rpcrt4.lib")


void gen_guid(unsigned char *pguid){

	
	UUID uuid;
	UuidCreate(&uuid);

	UuidToStringA(&uuid,&pguid);


	return;
	//return pguid;

}

//创建一个guid字符串
std::string new_guid(void)
{
	UUID uuid;
	UuidCreate( &uuid);

	unsigned char * str;
	UuidToStringA( &uuid, &str);

	std::string s( (char*)str );
	RpcStringFreeA( &str );

	return s;
}

//GUID->string
void guid_to_string(GUID *uuid,std::string & s)
{	
	unsigned char * str;
	UuidToStringA( uuid, &str);

	s = (char*)str;
	RpcStringFreeA( &str );
}

//string->GUID
void string_to_guid(std::string &d,GUID *uuid)
{
	UuidFromStringA((unsigned char *)(LPCTSTR)d.c_str(),uuid);
}
