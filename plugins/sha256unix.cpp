#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int sha256unix_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if((strlen(hashline)>=47 || strlen(hashline)<=91) && isStartsWith(hashline,"$5"))
	{
		std::vector<char*> v = split(line,"$");
		if((v.size() == 3) && (strlen(v[0]) == 1) && isAlphaDotSlash(v[1]) && isAlphaDotSlash(v[2]))
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

int sha256unix_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int sha256unix_check_valid(struct crack_hash* hash)
{
	if (!hash) 
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash)>=47 || strlen(hash->hash)<=91) && isStartsWith(hash->hash,"$5"))
		return 1;
	else
		return 0;
}

int sha256unix_is_special()
{
	return 0;
}