#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int bfunix_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if(isStartsWith(hashline,"$2$")||isStartsWith(hashline,"$2a$")||isStartsWith(hashline,"$2y$"))
	{
		std::vector<char*> v = split(line,"$");
		if((v.size() == 3) && strlen(v[2])==53 && isAlphaDotSlash(v[2]))
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

int bfunix_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int bfunix_check_valid(struct crack_hash* hash)
{
	return !bfunix_parse_hash(hash->hash,NULL,hash);
}

int bfunix_is_special()
{
	return 0;
}