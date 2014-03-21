#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int joomla_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
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

int joomla_check_valid(struct crack_hash* hash)
{
	if((strlen(hash->hash) == 32) && ishex(hash->hash))
		return 1;
	else
		return 0;
}

int joomla_is_special()
{
	return 0;
}