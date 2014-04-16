#include <stdio.h>
#include <string.h>
#include <sstream>
#include <ctype.h>
#include "trex/trex.h"
#include "str_api.h"
#include "plugin.h"
#include "algorithm_types.h"
#include <algorithm>

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

int isbase64(char *hashline)
{
	const TRexChar *error = NULL;
	TRex *x = trex_compile(_TREXC("^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$"),&error);
	TRexChar *line = _TREXC(hashline);
	if(x) 
	{
		if(trex_match(x,line))
		{
			trex_free(x);
			return 1;
		}
		else 
		{
			trex_printf(_TREXC("no match!\n"));
			trex_free(x);
			return 0;
		}	
	}
	else 
	{
		trex_printf(_TREXC("compilation error [%s]!\n"),error?error:_TREXC("undefined"));
		return 0;
	}	
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

size_t countSpecChar(char *hashline, char c)
{
	std::string s = hashline;
	return std::count(s.begin(), s.end(), c);
}

int isupperhex(char *hashline)
{
	char line[HASHFILE_MAX_LINE_LENGTH];
	snprintf(line, HASHFILE_MAX_LINE_LENGTH-1, "%s", hashline);

	for (int i=0;i<strlen(line);i++) 
		if (!isxdigit(line[i])) 
		{
			return 0;
		}
		else
		{
			if(isalpha(line[i]) && !isupper(line[i]))
			{
				return 0;
			}
		}

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

char* atoh(unsigned char *src, size_t size)
{
	int i = 0;

	char *dest = (char*)malloc(size+1);
	
	while(i<size)
	{
		snprintf(dest+i,2,"%02X",(unsigned int)*(src+i/2));
		i+=2;
	}
	*(dest+size)='\0';
	return dest;
}