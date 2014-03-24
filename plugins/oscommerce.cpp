#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int oscommerce_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if(strlen(hashline)>=33)
	{
		std::vector<char*> v = split(line,":");
		if((strlen(v[0]) == 32) && ishex(v[0]))
		{
			strcpy(hash->hash, v[0]);
			if(v.size() == 2)
				strcpy(hash->salt, v[1]);
			else
				strcpy(hash->salt, "");
			strcpy(hash->salt2, "");
			return 0;
		}
		else
			return 1;
	}
	else
		return 1;
}

int oscommerce_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	/*if(strcmp(hash->salt, "") == 0)
		snprintf(line, size, "%s:", hash->hash);
	else*/
		snprintf(line, size, "%s:%s", hash->hash, hash->salt);
	
	return 0;
}

int oscommerce_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash) == 32) && ishex(hash->hash))
		return 1;
	else
		return 0;
}

int oscommerce_is_special()
{
	return 0;
}