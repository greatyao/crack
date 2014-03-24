#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int vbulletin_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	return oscommerce_parse_hash(hashline,filename,hash);
}

int vbulletin_check_valid(struct crack_hash* hash)
{
	return oscommerce_check_valid(hash);
}

int vbulletin_recovery(const struct crack_hash* hash, char* line, int size)
{
	return oscommerce_recovery(hash,line,size);
}

int vbulletin_is_special()
{
	return 0;
}