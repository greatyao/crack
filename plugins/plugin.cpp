#include "plugin.h"
#include "algorithm_types.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct hash_support_plugins all_plugins[] = 
{
	{algo_md4, &md4_parse_hash, &md4_is_special, &md4_check_valid, &md4_recovery},
	{algo_md5, &md5_parse_hash, &md5_is_special, &md5_check_valid, &md5_recovery},
	{algo_md5md5, &md5md5_parse_hash, &md5md5_is_special, &md5md5_check_valid, &md5md5_recovery},
	{algo_md5unix, &md5unix_parse_hash, &md5unix_is_special, &md5unix_check_valid, &md5unix_recovery},
	{algo_oscommerce, &oscommerce_parse_hash, &oscommerce_is_special, &oscommerce_check_valid, &oscommerce_recovery},
	{algo_ipb2, &ipb2_parse_hash, &ipb2_is_special, &ipb2_check_valid, &ipb2_recovery},
	{algo_joomla, &joomla_parse_hash, &joomla_is_special, &joomla_check_valid,&joomla_recovery},
	{algo_vbulletin, &vbulletin_parse_hash, &vbulletin_is_special, &vbulletin_check_valid,&vbulletin_recovery},
	{algo_desunix, &desunix_parse_hash, &desunix_is_special, &desunix_check_valid,&desunix_recovery},
	{algo_sha1, &sha1_parse_hash, &sha1_is_special, &sha1_check_valid,&sha1_recovery},
	{algo_sha1sha1, &sha1sha1_parse_hash, &sha1sha1_is_special, &sha1sha1_check_valid,&sha1sha1_recovery},
	{algo_sha256, &sha256_parse_hash, &sha256_is_special, &sha256_check_valid,&sha256_recovery},
	{algo_sha512, &sha512_parse_hash, &sha512_is_special, &sha512_check_valid,&sha512_recovery},
	{algo_mysql5, &mysql5_parse_hash, &mysql5_is_special, &mysql5_check_valid,&mysql5_recovery},
	{algo_ntlm, &ntlm_parse_hash, &ntlm_is_special, &ntlm_check_valid,&ntlm_recovery},
	{algo_lm, &lm_parse_hash, &lm_is_special, &lm_check_valid,&lm_recovery},
	{algo_wordpress, &wordpress_parse_hash, &wordpress_is_special, &wordpress_check_valid,&wordpress_recovery},
	{algo_apr1, &apr1_parse_hash, &apr1_is_special, &apr1_check_valid,&apr1_recovery},
	{algo_mssql_2000, &mssql_2000_parse_hash, &mssql_2000_is_special, &mssql_2000_check_valid,&mssql_2000_recovery},

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

    for(i=0; i<theLength; ++i) 
	{
		if(ioString[i]>65 && ioString[i]<90)
			ioString[i] = tolower(ioString[i]);
	}
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
