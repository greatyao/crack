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
	if(strlen(hashline)==35)
	{
		std::vector<char*> v = split(line,":");
		if((v.size() == 2) && (strlen(v[0]) == 32) && (strlen(v[1]) == 2) && ishex(v[0]) && ishex(v[1]))
		{
			strcpy(hash->hash, v[0]);
			strcpy(hash->salt, v[1]);
			strcpy(hash->salt2, "");
			return 0;
		}
		else
			return 1;
	}
	else
		return 1;
}

int oscommerce_check_valid(struct crack_hash* hash)
{
	if((strlen(hash->hash) == 32) && (strlen(hash->salt) == 2) && ishex(hash->hash) && ishex(hash->salt))
		return 1;
	else
		return 0;
}

int oscommerce_is_special()
{
	return 0;
}