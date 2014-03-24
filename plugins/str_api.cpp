#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ctype.h>
#include "str_api.h"
#include "algorithm_types.h"

int ishex(char *hashline)
{
	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);

	for (int i=0;i<strlen(line);i++) 
		if (!isxdigit(line[i])) 
			return 0;

	return 1;
}

int isAlphaDotSlash(char *hashline)
{
	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);

	for (int i=0;i<strlen(line);i++) 
		if (!isalnum(line[i]) && '.'!=line[i] && '/'!=line[i]) 
			return 0;

	return 1;
}

int isStartsWith(char *hashline, char *str)
{
	std::string line = hashline;
	if(line.find(str)==0)
		return 1;
	else
		return 0;
}

int isEndsWith(char *hashline, char *str)
{
	std::string line = hashline;
	if(line.find(str)==(strlen(hashline)-strlen(str)))
		return 1;
	else
		return 0;
}

int isContains(char *hashline, char *str)
{
	std::string line = hashline;
	if(line.find(str)!=std::string::npos)
		return 1;
	else
		return 0;
}

int isupperhex(char *hashline)
{
	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);

	for (int i=0;i<strlen(line);i++) 
		if (isalpha(line[i]) && !isupper(line[i])) 
			return 0;

	return 1;
}

std::vector<char*> split(char *hashline, char *str)
{
	char *p;
	char *c = strtok_r(hashline,str, &p);
	std::vector<char*> v;

	int i=0;
	while(c!=NULL)
	{
		v.push_back(c);
		c = strtok_r(NULL,str, &p);
	}
	return v;
}