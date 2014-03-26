#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ctype.h>
#include "trex/trex.h"
#include "str_api.h"
#include "plugin.h"
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
//^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$

//#include <regex>
//int isbase64(char *hashline)
//{
//	std::tr1::cmatch res;
//    std::tr1::regex rx("^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$");
//    std::tr1::regex_search(hashline, res, rx);
//	return res.size();
//}

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