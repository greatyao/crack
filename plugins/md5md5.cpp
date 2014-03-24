#include <stdio.h>
#include <string.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int md5md5_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
    return md4_parse_hash(hashline,filename,hash);
}

int md5md5_check_valid(struct crack_hash* hash)
{
	return md4_check_valid(hash);
}

int md5md5_recovery(const struct crack_hash* hash, char* line, int size)
{
	return md4_recovery(hash,line,size);
}

int md5md5_is_special()
{
    return 0;
}