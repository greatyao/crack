#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int oracle_old_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if(countSpecChar(hashline,':')==1)
	{
		std::vector<char*> v = split(line,":");
		int i = v.size()-1;
		if((strlen(v[i]) == 16) && ishex(v[i]))
		{
			strcpy(hash->hash, v[i]);
			strcpy(hash->salt, "");
			if(i == 1)
				strcpy(hash->salt2, v[0]);
			else
				strcpy(hash->salt2, "");
			return 0;
		}
	}
	else if(strlen(hashline)==16 && ishex(hashline))
	{
		strcpy(hash->hash, hashline);
		strcpy(hash->salt, "");
		strcpy(hash->salt2, "");
		return 0;
	}
	return 1;
}

int oracle_old_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	if(strcmp(hash->salt2, "") == 0)
		snprintf(line, size, "%s", hash->hash);
	else
		snprintf(line, size, "%s:%s", hash->salt2, hash->hash);

	return 0;
}

int oracle_old_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash) == 16) && ishex(hash->hash))
		return 1;
	else
		return 0;
}

int oracle_old_is_special()
{
	return 0;
}