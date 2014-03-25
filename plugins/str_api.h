#ifndef _STR_API_H_
#define _STR_API_H_

#include <vector>

#if defined(WIN32) || defined(WIN64)
#if _MSC_VER <= 1500
#define snprintf _snprintf
#define strtok_r strtok_s
#define bzero(a, s) memset(a, 0, s)
#endif
#endif

int ishex(char *hashline);
int isAlphaDotSlash(char *hashline);
int isbase64(char *hashline);
int isStartsWith(char *hashline, char *str);
int isEndsWith(char *hashline, char *str);
int isContains(char *hashline, char *str);
int isupperhex(char *hashline);
std::vector<char*> split(char *hashline, char *str);

#endif