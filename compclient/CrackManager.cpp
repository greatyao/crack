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
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/stat.h>

static string filedb_path;
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
	if(tools)
	{
		for(int i = 0; i < toolCount; i++)
		{
			if(tools[i]) delete tools[i];
		}
		delete []tools;
	}
}
	
static int to_lower(int c)  
{  
    if (isupper(c))  
    {  
		return c+32;  
    }  
	return c;  
}  

	
int CrackManager::Init()
{
	string value;
	
	Config::Get().GetValue("files_path", value);
	if(value == "")
		value = "files_db";
	filedb_path = value;
	if(access(filedb_path.c_str(), 0) != 0)
		mkdir(filedb_path.c_str(), 0777);
		
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
		std::transform(vv.begin(), vv.end(), vv.begin(), to_lower);
		
		if(access(value.c_str(), 0) != 0)
		{
			CLog::Log(LOG_LEVEL_WARNING, "CrackManager: Couldn't find Crack tool execute file %s\n", value.c_str());
			continue;
		}
		
		if(vv.find("hashkill") != string::npos)
		{
			tools[i] = new HashKill();
			tools[i]->SetPath(value.c_str());
			CLog::Log(LOG_LEVEL_NOMAL, "CrackManager: Crack tool HashKill\n");
		}
		else if(vv.find("oclhashcat") != string::npos)
		{
			tools[i] = new oclHashcat();
			tools[i]->SetPath(value.c_str());
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
			CLog::Log(LOG_LEVEL_ERROR, "CrackManager: crack_priority is invalid\n");
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
		if(tools[i]->RunningTasks() == 0 || tools[i]->SupportMultiTasks() != 0)
			return true;
	}
	return false;
}

int CrackManager::StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short* deviceIds, int ndevices)
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
	
	CLog::Log(LOG_LEVEL_NOMAL, "CrackManager: Starting to launch task [guid=%s]\n", guid);
	//static int a = 0;
	//if(++a % 2 == 0) return ERR_LAUCH_TASK;
	
#if 0
	return tools[toolPriority]->StartCrack(item, guid, gpu, deviceIds, ndevices);
#else	
	for(int i = 0; i < toolCount; i++)
	{
		if(!tools[i])
			continue;
			
		if(tools[i]->RunningTasks() != 0 && tools[i]->SupportMultiTasks() == 0)
			continue;
		
		if(tools[i]->StartCrack(item, guid, gpu, deviceIds, ndevices) == 0)
			return 0;
	}
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