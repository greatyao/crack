#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"


int ripemd160_parse_hash(char *hashline, char *filename, struct crack_hash* hash0)
{
    char username[HASHFILE_MAX_LINE_LENGTH];
    char hash[HASHFILE_MAX_LINE_LENGTH];
    char line[HASHFILE_MAX_LINE_LENGTH];
    char *temp_str = NULL;
    
    if (!hashline || !hash) 
		return ERR_INVALID_PARAM;
    
    if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;
    
    snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
    if (strstr(line,":"))
    {
		strcpy(username, strtok(line, ":"));
		temp_str=strtok(NULL,":");
    }
    if (temp_str) 
    {
		strcpy(hash, temp_str);
    }
    else
    {
		strcpy(hash,line);
		strcpy(username,"N/A");
    }

    /* Hash is not 40 characters long => not a ripemd160 hash */
    if (strlen(hash)!=40)
    {
		return ERR_INVALID_PARAM;
    }
    
    strcpy(hash0->hash, hash);
	strcpy(hash0->salt, "");
	strcpy(hash0->salt2, "");

    return 0;
}


int ripemd160_check_valid(struct crack_hash* hash)
{
    if(!hash)
		return ERR_INVALID_PARAM;
	if(strlen(hash->hash) == 40)
		return 1;
	else
		return 0;
}


int ripemd160_recovery(const struct crack_hash* hash, char* line, int size)
{
   if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int ripemd160_is_special()
{
	return 0;
}