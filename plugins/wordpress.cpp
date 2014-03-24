#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int wordpress_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if(strlen(hashline)==34 && isStartsWith(hashline,"$P"))
	{
		std::vector<char*> v = split(line,"$");
		if((v.size() == 2) && (strlen(v[0]) == 1) && (strlen(v[1]) == 31) && isAlphaDotSlash(v[1]))
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

int wordpress_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int wordpress_check_valid(struct crack_hash* hash)
{
	if (!hash) 
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash) == 34) && isStartsWith(hash->hash,"$P"))
		return 1;
	else
		return 0;
}

int wordpress_is_special()
{
	return 0;
}