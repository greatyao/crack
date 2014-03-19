#include "plugin.h"
#include "algorithm_types.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct hash_support_plugins all_plugins[] = 
{
	{algo_md5, &md5_parse_hash, &md5_is_special, &md5_check_valid},
	//请在这里相应的添加各种插件
	{0, NULL, NULL, NULL}
};

#define PLUGINS_NUMBER sizeof(all_plugins)/sizeof(all_plugins[0])

struct hash_support_plugins* locate_by_algorithm(int algo)
{
	for(int i = 0; i < PLUGINS_NUMBER; i++)
		if(algo == all_plugins[i].algo)	
			return &(all_plugins[i]);
			
	return NULL;
}

#ifndef __CYGWIN__
char* strupr(char* ioString)
{
    int i;
    int theLength = (int)strlen(ioString);

    for(i=0; i<theLength; ++i) {ioString[i] = toupper(ioString[i]);}
    return ioString;
}
#endif

char* strlow(char* ioString)
{
    int i;
    int theLength = (int)strlen(ioString);

    for(i=0; i<theLength; ++i) {ioString[i] = tolower(ioString[i]);}
    return ioString;
}


void hex2str(char *str, char *hex, int len)
{
    int cnt, cnt1;
    unsigned char val=0;
    unsigned char tmp1=0,tmp2=0;
    char *charset="0123456789abcdef";
    bzero(str, (len/2));

    for (cnt=0;cnt<(len/2);cnt++)
    {
		val = 0;
		for (cnt1=0;cnt1<16;cnt1++) if (charset[cnt1] == hex[cnt*2]) tmp1 = cnt1;
		for (cnt1=0;cnt1<16;cnt1++) if (charset[cnt1] == hex[cnt*2+1]) tmp2 = cnt1;
		val |= (tmp1 << 4);
		val |= tmp2;
		
		*(str+cnt) = val & 255;
    }
}


char itoa64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
void _to64(char *s, unsigned long v, int n)
{

	while (--n >= 0) {
			*s++ = itoa64[v&0x3f];
			v >>= 6;
	}
}
