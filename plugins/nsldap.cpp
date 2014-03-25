#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int nsldap_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if((strlen(hashline)==33) && isStartsWith(hashline,"{SHA}") && isbase64(hashline+5))
	{
		strcpy(hash->hash, hashline);
		strcpy(hash->salt, "");
		strcpy(hash->salt2, "");
		return 0;
	}
	else
		return 1;
}

int nsldap_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;
	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int nsldap_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;
	if((strlen(hash->hash)==33) && isStartsWith(hash->hash,"{SHA}") && isbase64(hash->hash+5))
		return 1;
	else
		return 0;
}

int nsldap_is_special()
{
	return 0;
}