#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int desunix_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if((strlen(line) == 13) && isAlphaDotSlash(line))
	{
		strcpy(hash->hash, line);
		strcpy(hash->salt, "");
		strcpy(hash->salt2, "");
		return 0;
	}
	else
		return 1;
}

int desunix_check_valid(struct crack_hash* hash)
{
	if((strlen(hash->hash) == 13) && isAlphaDotSlash(hash->hash))
		return 1;
	else
		return 0;
}

int desunix_is_special()
{
	return 0;
}