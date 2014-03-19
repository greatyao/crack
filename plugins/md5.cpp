#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"

int md5_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
    char line[HASHFILE_MAX_LINE_LENGTH];
    char *temp_str = NULL;
    
    if (!hashline || !hash) 
		return ERR_INVALID_PARAM;
    
    if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;
    
    snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
 
	/* Hash is not 32 characters long => not a md5 hash */
	if (strlen(line)!=32)
		return ERR_INVALID_PARAM;
	strcpy(hash->hash, line);
    
    strcpy(hash->salt, "");
    strcpy(hash->salt2, "");

    return 0;
}

int md5_check_valid(struct crack_hash* hash)
{
	if(strlen(hash->hash) != 32)
		return 0;
		
	char temp[32+1];
	strcpy(temp, hash->hash);	
	strlow(temp);
	for (int a=0;a<32;a++) 
		if ( ((temp[a]<'0')||(temp[a]>'9'))&&((temp[a]<'a')||(temp[a]>'f'))) 
			return 0;
	
	return 1;
}

int md5_is_special()
{
    return 0;
}