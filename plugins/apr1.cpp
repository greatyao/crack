#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int apr1_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if(strlen(hashline)==37 && isStartsWith(hashline,"$apr1"))
	{
		std::vector<char*> v = split(line,"$");
		if((v.size() == 3) && (strlen(v[0]) == 4) && (strlen(v[2]) == 22) && isAlphaDotSlash(v[1]) && isAlphaDotSlash(v[2]))
		{
			strcpy(hash->hash, hashline);
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

int apr1_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int apr1_check_valid(struct crack_hash* hash)
{
	if (!hash) 
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash) == 37) && isStartsWith(hash->hash,"$apr1"))
		return 1;
	else
		return 0;
}

int apr1_is_special()
{
	return 0;
}