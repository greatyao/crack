/* Config.cpp
 *
 * Loading Configuration
 * Copyright (C) 2014 TRIMPS
 *
 * Created By YAO Wei at  03/21/2014
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "Config.h"
#include "err.h"

#define COMMENT_CHAR '#'

static bool IsSpace(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

static bool IsEnter(char c)
{
    if ('\r' == c || '\n' == c)
        return true;
    return false;
}

static bool IsCommentChar(char c)
{
    switch(c) {
    case COMMENT_CHAR:
        return true;
    default:
        return false;
    }
}

static void Trim(string & str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }
    
    start_pos = i;
    
    for (i = str.size() - 1; i >= 0; --i) {
        if (!IsSpace(str[i]) && !IsEnter(str[i])) {
            break;
        }
    }
    end_pos = i;
    
    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool AnalyseLine(const string & line, string & key, string & value)
{
    if (line.empty())
        return false;
    int start_pos = 0, end_pos = line.size() - 1, pos;
    if ((pos = line.find(COMMENT_CHAR)) != -1) {
        if (0 == pos) {  // 行的第一个字符就是注释字符
            return false;
        }
        end_pos = pos - 1;
    }
    string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分
    
    if ((pos = new_line.find('=')) == -1)
        return false;  // 没有=号
        
    key = new_line.substr(0, pos);
    value = new_line.substr(pos + 1, end_pos + 1- (pos + 1));
    
    Trim(key);
    if (key.empty()) {
        return false;
    }
    Trim(value);
    return true;
}

Config& Config::Get()
{
	static Config config;
	return config;
}
	
Config::Config()
{
}

Config::~Config()
{
}

int Config::GetValue(const string& key, string& value)
{
	if(values.find(key) == values.end())
	{
		value = "";
		return ERR_NOENTRY;
	}
	
	value = values[key];
	return 0;
}

int Config::ReadConfig(const string & filename)
{
	char path[256];
	readlink("/proc/self/exe", path, sizeof(path));
	char* p = path + strlen(path) -1;
	while(p > path){
		if(*p == '/' || *p == '\\'){
			*p = 0;
			break;
		}
		p--;
	}
	
	string fn = string(path) + "/" + filename;
	FILE* file = fopen(fn.c_str(), "r");
    if (file == NULL)
    {
		fprintf(stderr, "Cannot open config file: %s\n", filename.c_str());
		exit(0);
    }
    
	char line[512];
	string key, value;
	values.clear();
	while (!feof(file))
	{
	    if (fgets((char *)&line, 512, file) != NULL)
	    {
			if (AnalyseLine(line, key, value)) 
			{
				values[key] = value;
			}		
	    }
	}
    
    fclose(file);
    return 0;
}