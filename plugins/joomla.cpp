#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int joomla_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	return oscommerce_parse_hash(hashline,filename,hash);
}

int joomla_check_valid(struct crack_hash* hash)
{
	return oscommerce_check_valid(hash);
}

int joomla_recovery(const struct crack_hash* hash, char* line, int size)
{
	return oscommerce_recovery(hash,line,size);
}

int joomla_is_special()
{
	return 0;
}