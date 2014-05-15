// TestServer.h : main header file for the PROJECT_NAME application
//
#pragma once

#include <vector>
#include <map>
#include <deque>
#include <string>

using std::string;
class CClientInfo;
class CCrackTask;
class CBlockNotice;
class CCrackBlock;
class CCrackHash;
	 
struct MapLessCompare{
	bool operator()(const char * str1,const char *str2) const
	{
		return strcmp(str1,str2) < 0;
	}

	bool operator()(const string& str1,const string& str2) const
	{
		return strcmp(str1.c_str(),str2.c_str()) < 0;
	}
};

//typedef std::map<char *, CCrackBlock *,MapLessCompare > TOTAL_CB_MAP;

typedef std::vector<char *> GUID_VECTOR;

typedef std::vector<CBlockNotice *> CBN_VECTOR;

typedef std::vector<CClientInfo *> CI_VECTOR;

typedef std::map<char *,CCrackTask *,MapLessCompare> CT_MAP;

typedef std::map<string,CBN_VECTOR,MapLessCompare> CCB_MAP; //computer<----> block ”≥…‰

//typedef std::queue<char *> CT_QUEUE;
typedef std::deque<char *> CT_DEQUE;

struct CBMapLessCompare{
	
	bool operator()(const char * str1,const char *str2) const
	{

		return strcmp(str1,str2) < 0;
	}
};

typedef std::map<char *, CCrackBlock *,CBMapLessCompare> CB_MAP;
typedef std::vector<CCrackHash *> CRACK_HASH_LIST;

typedef std::map<string, CCrackTask *, MapLessCompare> CT_MAP2;
