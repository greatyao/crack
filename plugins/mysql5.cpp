#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int mysql5_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!hashline || !hash) 
		return ERR_INVALID_PARAM;

	if (strlen(hashline)<2) 
		return ERR_INVALID_PARAM;

	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);
	if(((strlen(hashline)==41) && isStartsWith(hashline,"*") && isupperhex(hashline+1)) || ((strlen(hashline)==16)&&ishex(hashline)))
	{
		if(ishex(hashline+1))
		{
			if(strlen(hashline)==41)
			{
				strcpy(hash->hash, hashline+1);
			}
			else
			{
				strcpy(hash->hash, hashline);
			}
			
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

int mysql5_check_valid(struct crack_hash* hash)
{
	if(((strlen(hash->hash) == 40) && ishex(hash->hash) && isupperhex(hash->hash)) || ((strlen(hash->hash) == 16) && ishex(hash->hash)))
		return 1;
	else
		return 0;
}

int mysql5_is_special()
{
	return 0;
}