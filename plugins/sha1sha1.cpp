#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int sha1sha1_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	return sha1_parse_hash(hashline,filename,hash);
}

int sha1sha1_recovery(const struct crack_hash* hash, char* line, int size)
{
	return sha1_recovery(hash,line,size);
}

int sha1sha1_check_valid(struct crack_hash* hash)
{
	return sha1_check_valid(hash);
}

int sha1sha1_is_special()
{
	return 0;
}