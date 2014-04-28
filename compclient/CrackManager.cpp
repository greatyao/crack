/* CrackManager.cpp
 *
 * Coordnator among several crack algorithms
 * Copyright (C) 2014 TRIMPS
 *
 * Craeted By YAO Wei at  03/21/2014
 */

#include "CrackManager.h"
#include "Crack.h"
#include "oclHashcat.h"
#include "HashKill.h"
#include "CLog.h"
#include "Config.h"
#include "algorithm_types.h"
#include "Client.h"
#include "err.h"
#include "plugin.h"

#include <ctype.h>
#include <errno.h>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/stat.h>

static string filedb_path;
static string dict_path;
static bool using_cpu;

CrackManager& CrackManager::Get()
{
	static CrackManager manager;
	return manager;
}
	
CrackManager::CrackManager()
{
	tools = NULL;
	toolCount = 0;
	toolPriority = -1;
	
}

CrackManager::~CrackManager()
{
	Destroy();
}
	
void CrackManager::Destroy()
{
	if(tools)
	{
		for(int i = 0; i < toolCount; i++)
		{
			if(tools[i]) delete tools[i];
		}
		delete []tools;
		toolCount = 0;
		tools = NULL;
	}	
}	

int CrackManager::Init()
{
	string value;
	
	Config::Get().GetValue("files_path", value);
	if(value == "")
		value = "/var/crack_files";
	filedb_path = value;
	if(access(filedb_path.c_str(), 0) != 0 && mkdir(filedb_path.c_str(), 0666)!=0)
	{
		CLog::Log(LOG_LEVEL_ERROR, "CrackManager: Couldn't mkdir %s:[%s]\n", 
			filedb_path.c_str(), strerror(errno));
		return ERR_FAILED_MKDIR;
	}	
		
	Config::Get().GetValue("dict_path", value);
	if(value == "")
		value = "/var/crack_dict";
	dict_path = value;
	if(access(dict_path.c_str(), 06) != 0 && mkdir(dict_path.c_str(), 0666) != 0)
	{
		CLog::Log(LOG_LEVEL_ERROR, "CrackManager: Couldn't mkdir %s:[%s]\n", 
			dict_path.c_str(), strerror(errno));
		return ERR_FAILED_MKDIR;
	}
	
		
	Config::Get().GetValue("using_cpu", value);
	if(value == "" || value == "0" || value == "false")
		using_cpu = false;
	else
		using_cpu = true;
	
	if(Config::Get().GetValue("crack_tools_count", value) != 0 ||
		(toolCount = atoi(value.c_str())) < 0)
	{
		CLog::Log(LOG_LEVEL_ERROR, "CrackManager: Couldn't find crack_tools_count in config file\n");
		return ERR_NOENTRY;
	}
	
	CLog::Log(LOG_LEVEL_NOMAL, "CrackManager: Find %d crack tools in config file\n", toolCount);
	tools = new Crack* [toolCount];
	memset(tools, 0, sizeof(Crack*)*toolCount);
	for(int i = 0; i < toolCount; i++)
	{
		char v[64];
		sprintf(v, "crack_tool_%d_path", i);
		if(Config::Get().GetValue(v, value) != 0)
		{
			tools[i] = NULL;
			CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Couldn't find %s in config file\n", v);
			continue;
		}
		
		string vv = value;
		std::transform(vv.begin(), vv.end(), vv.begin(), ::tolower);
		
		if(access(value.c_str(), 0) != 0)
		{
			CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Couldn't find Crack tool execute file %s\n", value.c_str());
			tools[i] = NULL;
			continue;
		}
		
		if(vv.find("hashkill") != string::npos)
		{
			tools[i] = new HashKill();
			tools[i]->SetPath(value.c_str());
			tools[i]->SetToolName("HashKill");
			CLog::Log(LOG_LEVEL_NOMAL, "CrackManager: Crack tool HashKill\n");
		}
		else if(vv.find("oclhashcat") != string::npos)
		{
			tools[i] = new oclHashcat();
			tools[i]->SetPath(value.c_str());
			tools[i]->SetToolName("oclHashcat");
			CLog::Log(LOG_LEVEL_NOMAL, "CrackManager: Crack tool oclHashcat\n");
		}
		else
			tools[i] = NULL;
	}
	
	if(Config::Get().GetValue("crack_priority", value) != 0)
	{
		CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Couldn't find crack_priority in config file\n");
		toolPriority = 0;
	}
	else
	{
		toolPriority = atoi(value.c_str());
	}
	
	if(tools[toolPriority] == NULL)
	{
		int i;
		for(i = 0; i < toolCount; i++)
		{
			if(tools[i])
			{
				toolPriority = i;
				break;
			}
		}
		if(i == toolCount)
		{
			CLog::Log(LOG_LEVEL_ERROR, "CrackManager: crack_priority is invalid, please reconfig\n");
			return ERR_NOENTRY;
		}
		else
		{
			CLog::Log(LOG_LEVEL_WARNING, "CrackManager: crack_priority invalid, but we choose a valid one\n");
		}
	}
	
	return 0;
}

void CrackManager::RegisterCallback(int (*done)(char*, bool, const char*, bool), 
									int (*status)(char*, int, float, unsigned int))
{
	for(int i = 0; i < toolCount; i++)
	{
		if(tools && tools[i])
			tools[i]->RegisterCallback(done, status);
	}
}

void CrackManager::GetFilename(const char* guid, char* filename, int size)const
{
	snprintf(filename, size, "%s/%s", filedb_path.c_str(), guid);
}

bool CrackManager::GetDict(unsigned char dict, char* dict_name, int size, char* rule_name, int size2)const
{
	snprintf(dict_name, size, "%s/dict-%d.txt", dict_path.c_str(), dict);
	if(access(dict_name, 0) != 0) 
	{
		CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Couldn't access dict_file %s\n", dict_name);
		return false;
	}
	
	if(rule_name && size2 != 0)
	{
		snprintf(rule_name, size2, "%s/rule_%d", dict_path.c_str(), dict);
		if(access(rule_name, 0) != 0)
		{
			FILE* f = fopen(rule_name, "w");
			if(!f)
			{
				CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Couldn't create rule_file %s\n", rule_name);
				return false;
			}
			
			fprintf(f, "begin\nmust add dict %s\nend\n", dict_name);
			fclose(f);
		}
	}
	
	return true;
}

bool CrackManager::UsingCPU()const
{
	return using_cpu;
}

int CrackManager::CheckParameters(crack_block* item)
{
	//首先需要验证数据的有效性
	struct hash_support_plugins* plugin = locate_by_algorithm(item->algo);
	if(plugin){
		struct crack_hash hash;
		if(plugin->special() == 0)
		{
			if(plugin->parse((char*)item->john, NULL, &hash) != 0)
			{
				CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Invalid hash format %s\n", item->john);
				return ERR_INVALID_PARAM;
			}
		}
	}
	
	char file[512];
	GetFilename(item->guid, file, sizeof(file));
	if(item->special != 0)
		strcpy(item->john, file);
	
	return 0;
}

bool CrackManager::CouldCrack()const
{
	if(!tools)
		return false;
	
	for(int i = 0; i < toolCount; i++)
	{
		if(!tools[i]) 
			continue;
		if(tools[i]->RunningTasks() == 0 || tools[i]->SupportMultiTasks() != 0)
			return true;
	}
	return false;
}

int CrackManager::StartCrack(const crack_block* item, const char* guid, bool gpu, 
							unsigned short* deviceIds, int ndevices,
							char* toolname, int size)
{
	if(!tools || !tools[toolPriority])
		return ERR_NOENTRY;
	
	bool download = true;
	if(item->special !=0 && access(item->john, 0) != 0)
	{
		CLog::Log(LOG_LEVEL_NOTICE, "CrackManager: This is file crack, fetch original file\n");
		int ntry = 0;
		while(1)
		{
			download = (Client::Get().DownloadFile(item->guid, filedb_path.c_str()) == 0);
			if(++ntry >= 3 || download == true)	break;
		}
	}

	if(!download)
	{	
		CLog::Log(LOG_LEVEL_ERROR, "CrackManager: Could NOT download file [guid=%s]\n", guid);
		return ERR_DOWNLOADFILE;
	}
	
	CLog::Log(LOG_LEVEL_NOMAL, "CrackManager: Starting to launch task [guid=%s, algo=%d, type=%d]\n", 
		guid, item->algo, item->type);
	//static int a = 0;
	//if(++a % 2 == 0) return ERR_LAUCH_TASK;
	
#if 0
	return tools[toolPriority]->StartCrack(item, guid, gpu, deviceIds, ndevices);
#else
	static int base = 0;
	for(int i = base, j = 0; j < toolCount; i++, j++)
	{
		if(i >= toolCount) i -= toolCount;
		
		if(!tools[i])
			continue;
			
		if(tools[i]->RunningTasks() != 0 && tools[i]->SupportMultiTasks() == 0)
			continue;
		
		if(tools[i]->StartCrack(item, guid, gpu, deviceIds, ndevices) == 0)
		{
			base = (i+1)%toolCount;
			if(toolname && size)
				strncpy(toolname, tools[i]->toolname, size);
			return 0;
		}
	}
	base = (base + 1)%toolCount;
	
	return ERR_LAUCH_TASK;
#endif
}

int CrackManager::StopCrack(const char* guid)
{
#if 0
	if(!tools || !tools[toolPriority])
		return ERR_NOENTRY;
		
	return tools[toolPriority]->StopCrack(guid);
#else	
	if(!tools)
		return ERR_NOENTRY;
	for(int i = 0; i < toolCount; i++)
	{
		if(tools[i] && tools[i]->StopCrack(guid) == 0)
			return 0;
	}
	
	return ERR_FAILED_KILL;
#endif
}