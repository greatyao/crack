/* Config.h
 *
 * Loading Configuration
 * Copyright (C) 2014 TRIMPS
 *
 * Created By YAO Wei at  03/21/2014
 */
 
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <map>

using std::string;
using std::map;
 
class Config
{
public:
	static Config& Get();

	int ReadConfig(const string& filename);
	int GetValue(const string& key, string& value);
	
private:
	Config();
	~Config();
	
	map<string, string> values;
};

#endif