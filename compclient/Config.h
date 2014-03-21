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