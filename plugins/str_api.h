#include <vector>

int ishex(char *hashline);
int isAlphaDotSlash(char *hashline);
int isStartsWith(char *hashline, char *str);
int isEndsWith(char *hashline, char *str);
int isContains(char *hashline, char *str);
int isupperhex(char *hashline);
std::vector<char*> split(char *hashline, char *str);