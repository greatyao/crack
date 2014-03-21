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
#include "Hashkill.h"
#include "CLog.h"
#include "Config.h"
#include "algorithm_types.h"
#include "err.h"

#include <ctype.h>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

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

void CrackManager::RegisterCallback(int (*done)(char*, bool, const char*), 
									int (*status)(char*, int, float, unsigned int))
{
	for(int i = 0; i < toolCount; i++)
	{
		if(tools && tools[i])
			tools[i]->RegisterCallback(done, status);
	}
}

	
int CrackManager::StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceId)
{
	if(!tools || !tools[toolPriority])
		return ERR_NOENTRY;
		
	return tools[toolPriority]->StartCrack(item, guid, gpu, deviceId);
}

int CrackManager::StopCrack(const char* guid)
{
	if(!tools || !tools[toolPriority])
		return ERR_NOENTRY;
		
	return tools[toolPriority]->StopCrack(guid);
}