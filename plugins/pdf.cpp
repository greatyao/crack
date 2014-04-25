#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"

int pdf_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	return 0;
}

int pdf_check_valid(struct crack_hash* hash)
{
	return 1;
}

int pdf_recovery(const struct crack_hash* hash, char* line, int size)
{
	return 0;
}

int pdf_is_special()
{
	return 1;
}