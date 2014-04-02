#ifndef _STR_API_H_
#define _STR_API_H_

#include <vector>

int ishex(char *hashline);
int isAlphaDotSlash(char *hashline);
int isbase64(char *hashline);
int isStartsWith(char *hashline, char *str);
int isEndsWith(char *hashline, char *str);
size_t countSpecChar(char *hashline, char c);
int isupperhex(char *hashline);
std::vector<char*> split(char *hashline, char *str);

#endif