#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int nsldaps_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if((strlen(hashline)==46) && isStartsWith(hashline,"{SSHA}") && isbase64(hashline+6))
	{
		strcpy(hash->hash, hashline);
		strcpy(hash->salt, "");
		strcpy(hash->salt2, "");
		return 0;
	}
	else
		return 1;
}

int nsldaps_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;
	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int nsldaps_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash)==46) && isStartsWith(hash->hash,"{SSHA}") && isbase64(hash->hash+6))
		return 1;
	else
		return 0;
}

int nsldaps_is_special()
{
	return 0;
}