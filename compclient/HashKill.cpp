/* HashKill.cpp
 *
 * Hashkill tool for cracking
 * Copyright (C) 2014 TRIMPS
 *
 * Craeted By YAO Wei at  03/17/2014
 */

#include "HashKill.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <float.h> 
#include <unistd.h>

using std::string;

struct hash_parameter{
	crack_algorithm algo;
	const char* params[max_value]; 		//暴力破解
};

static const char* charsets[] = {
	"num",			 //对应于crack_charset中的charset_num
	"lalpha",		//以此类似		
	"ualpha",
	"alpha",
	"lalphanum",
	"ualphanum",
	"alphanum",
	"ascii",
	"",
};

static struct hash_parameter all_support_hashes[] = 
{
	{algo_md4,			{"-p md4 -b%d:%d:%s %s %s", "-p md4 -r %s %s %s"}},
	{algo_md5,			{"-p md5 -b%d:%d:%s %s %s", "-p md5 -r %s %s %s"}},
	{algo_md5md5,		{"-p md5md5 -b%d:%d:%s %s %s", "-p md5md5 -r %s %s %s"}},
	{algo_md5unix,		{"-p md5unix -r brute -a %d:%d:%s %s %s", "-p md5unix -r %s %s %s"}},
	{algo_mediawiki,	{"-p mediawiki -b%d:%d:%s %s %s", "-p mediawiki -r %s %s %s"}},
	{algo_oscommerce,	{"-p oscommerce -b%d:%d:%s %s %s", "-p oscommerce -r %s %s %s"}},
	{algo_ipb2,			{"-p ipb2 -b%d:%d:%s %s %s", "-p ipb2 -r %s %s %s"}},
	{algo_joomla,		{"-p joomla -b%d:%d:%s %s %s", "-p joomla -r %s %s %s"}},
	{algo_vbulletin,    {"-p vbulletin -b%d:%d:%s %s %s", "-p vbulletin -r %s %s %s"}},
	{algo_desunix,      {"-p desunix -b%d:%d:%s %s %s", "-p desunix -r %s %s %s"}},
	{algo_sha1,         {"-p sha1 -b%d:%d:%s %s %s", "-p sha1 -r %s %s %s"}},
	{algo_sha1sha1,     {"-p sha1sha1 -b%d:%d:%s %s %s", "-p sha1sha1 -r %s %s %s"}},
	{algo_sha256,       {"-p sha256 -b%d:%d:%s %s %s", "-p sha256 -r %s %s %s"}},
	{algo_sha256unix,   {"-p sha256unix -b%d:%d:%s %s %s", "-p sha256unix -r %s %s %s"}},
	{algo_sha512,       {"-p sha512 -b%d:%d:%s %s %s", "-p sha512 -r %s %s %s"}},
	{algo_sha512unix,   {"-p sha512unix -b%d:%d:%s %s %s", "-p sha512unix -r %s %s %s"}},
	{algo_msoffice_old, {"-p msoffice-old -b%d:%d:%s %s %s", "-p msoffice-old -r %s %s %s"}},
	{algo_msoffice,     {"-p msoffice -b%d:%d:%s %s %s", "-p msoffice -r %s %s %s"}},
	{algo_django256,    {"-p django256 -r brute -a %d:%d:%s: %s %s", "-p django256 -r %s %s %s"}},
	{algo_zip,          {"-p zip -b%d:%d:%s %s %s", "-p zip -r %s %s %s"}},
	{algo_rar,          {"-p rar -b%d:%d:%s %s %s", "-p rar -r %s %s %s"}},
	{algo_apr1,         {"-p apr1 -r brute -a %d:%d:%s: %s %s", "-p apr1 -r %s %s %s"}},
	{algo_bfunix,       {"-p bfunix -b%d:%d:%s %s %s", "-p bfunix -r %s %s %s"}},
	{algo_dmg,          {"-p dmg -b%d:%d:%s %s %s", "-p dmg -r %s %s %s"}},
	{algo_drupal7,      {"-p drupa17 -b%d:%d:%s %s %s", "-p drupa17 -r %s %s %s"}},
	{algo_lm,           {"-p lm -b%d:%d:%s %s %s", "-p lm -r %s %s %s"}},
	{algo_luks,         {"-p luks -b%d:%d:%s %s %s", "-p luks -r %s %s %s"}},
	{algo_mscash,       {"-p mscash -b%d:%d:%s %s %s", "-p mscash -r %s %s %s"}},
	{algo_mscash2,      {"-p mscash2 -b%d:%d:%s %s %s", "-p mscash2 -r %s %s %s"}},
	{algo_mssql_2000,   {"-p mssql-2000 -b%d:%d:%s %s %s", "-p mssql-2000 -r %s %s %s"}},
	{algo_mssql_2005,   {"-p mssql-2005 -b%d:%d:%s %s %s", "-p mssql-2005 -r %s %s %s"}},
	{algo_mssql_2012,   {"-p mssql-2012 -b%d:%d:%s %s %s", "-p mssql-2012 -r %s %s %s"}},
	{algo_mysql5,       {"-p mysql5 -b%d:%d:%s %s %s", "-p mysql5 -r %s %s %s"}},
	{algo_nsldap,       {"-p nsldap -b%d:%d:%s %s %s", "-p nsldap -r %s %s %s"}},
	{algo_nsldaps,      {"-p nsldaps -b%d:%d:%s %s %s", "-p nsldaps -r %s %s %s"}},
	{algo_ntlm,         {"-p ntlm -b%d:%d:%s %s %s", "-p ntlm -r %s %s %s"}},
	{algo_o5logon,      {"-p o5logon -b%d:%d:%s %s %s", "-p o5logon -r %s %s %s"}},
	{algo_oracle_old,   {"-p oracle-old -b%d:%d:%s %s %s", "-p oracle-old -r %s %s %s"}},
	{algo_oracle11g,    {"-p oracle11g -r brute -a %d:%d:%s %s %s", "-p oracle11g -r %s %s %s"}},
	{algo_osx_old,      {"-p osx-old -b%d:%d:%s %s %s", "-p osx-old -r %s %s %s"}},
	{algo_osxlion,      {"-p osxlion -b%d:%d:%s %s %s", "-p osxlion -r %s %s %s"}},
	{algo_phpbb3,       {"-p phpbb3 -r brute -a %d:%d:%s %s %s", "-p phpbb3 -r %s %s %s"}},
	{algo_pixmd5,       {"-p pixmd5 -b%d:%d:%s %s %s", "-p pixmd5 -r %s %s %s"}},
	{algo_privkey,      {"-p privkey -b%d:%d:%s %s %s", "-p privkey -r %s %s %s"}},
	{algo_ripemd160,    {"-p ripemd160 -b%d:%d:%s %s %s", "-p ripemd160 -r %s %s %s"}},
	{algo_sapg,         {"-p sapg -b%d:%d:%s %s %s", "-p sapg -r %s %s %s"}},
	{algo_sl3,          {"-p sl3 -b%d:%d:%s %s %s", "-p sl3 -r %s %s %s"}},
	{algo_smf,          {"-p smf -b%d:%d:%s %s %s", "-p smf -r %s %s %s"}},
	{algo_wordpress,    {"-p wordpress -r brute -a %d:%d:%s %s %s", "-p wordpress -r %s %s %s"}},
	{algo_wpa,          {"-p wpa -b%d:%d:%s %s %s", "-p wpa -r %s %s %s"}},
};

#define SUPPORT_HASH_NUM sizeof(all_support_hashes)/sizeof(all_support_hashes[0])

HashKill::HashKill(void)
{
}

HashKill::~HashKill(void)
{
}

int HashKill::SupportMultiTasks()const
{
	return 1;
}

int HashKill::Launcher(const crack_block* item, bool gpu, unsigned short* deviceIds, int ndevices)
{
	unsigned char algo = item->algo;
	unsigned char charset = item->charset;
	unsigned short start = item->start;
	unsigned short end = item->end;
	unsigned char type = item->type;
	const char* hash = item->john;
	const char* fmt;
	int i, j;

	for(i = 0; i < SUPPORT_HASH_NUM; i++)
	{
		if(algo == all_support_hashes[i].algo && type < max_value && type >= 0)
		{
			fmt = all_support_hashes[i].params[type];
			break;
		}
	}
	if(i == SUPPORT_HASH_NUM)
	{
		//未找到指定解密类型
		return ERR_NO_SUPPORT_ALGO;
	}

	if(charset < charset_num || charset > charset_ascii)
	{
		//不支持自定义类型
		return ERR_NO_SUPPORT_CHARSET;
	}
	
	if(ndevices < 1 || ndevices > 16)
	{
		return ERR_INVALID_PARAM;
	}
	
	unsigned short platformId = deviceIds[0] >> 8;
	unsigned short ids[16] = {0};
	for(j = 0; j < ndevices; j++)
		ids[j] = deviceIds[j] & 0xff;

	char cmd[4096];
	char others[128];
	if(!gpu)
		sprintf(others, "-c -C %d", ndevices);
	else if(ndevices == 1)
		sprintf(others, "-t %d", platformId);
	else 
	{
		sprintf(others, "-t %d -A ", platformId);
		for(j = 0; j < ndevices-1; j++)
			sprintf(others, "%s%d,", others, ids[j]);
		sprintf(others, "%s%d", others, ids[j]);
	}

	string john = item->john;
	if(item->algo == algo_mscash)
	{
		int id = john.find(":");
		if(id != string::npos)
			john = john.substr(id+1, john.length()-id-1) + ":" + john.substr(0, id);
	}
	else if(item->algo == algo_mediawiki)
	{
		int id = john.find(":");
		if(id != string::npos)
			john = string("B:") + john.substr(id+1, john.length()-id-1) + ":" + john.substr(0, id);
	}
	
	if(type == bruteforce)
		sprintf(cmd, fmt, start, end, charsets[charset], others, john.c_str());
	else if(type == dict)
		sprintf(cmd, fmt, "", others, john.c_str());
	
	int pid = this->Exec(item->guid, path, cmd, MonitorThread, true, true, false);
	
	if(pid > 0){
		CLog::Log(LOG_LEVEL_NOTICE, "hashkill: Laucher OK [guid=%s, pid=%d]\n", item->guid, pid);
	}else{
		CLog::Log(LOG_LEVEL_ERROR, "hashkill: Laucher failed [guid=%s]\n", item->guid);
	}

	return pid;	
}

int HashKill::Launcher(const crack_block* item, bool gpu, unsigned short deviceId)
{
	unsigned short platformId = deviceId >> 8;
	deviceId &= 0xff;
	unsigned char algo = item->algo;
	unsigned char charset = item->charset;
	unsigned short start = item->start;
	unsigned short end = item->end;
	const char* hash = item->john;
	const char* fmt;
	int i;

	for(i = 0; i < SUPPORT_HASH_NUM; i++)
	{
		if(algo == all_support_hashes[i].algo)
		{
			fmt = all_support_hashes[i].params[0];
			break;
		}
	}
	if(i == SUPPORT_HASH_NUM)
	{
		//未找到指定解密类型
		return ERR_NO_SUPPORT_ALGO;
	}

	if(charset < charset_num || charset > charset_ascii)
	{
		//不支持自定义类型
		return ERR_NO_SUPPORT_CHARSET;
	}

	char cmd[4096];
	char others[128];
	if(!gpu)
		sprintf(others, "-c");
	else
		sprintf(others, "-t %d", platformId);

	string john = item->john;
	if(item->algo == algo_mscash)
	{
		int id = john.find(":");
		if(id != string::npos)
			john = john.substr(id+1, john.length()-id-1) + ":" + john.substr(0, id);
	}
	else if(item->algo == algo_mediawiki)
	{
		int id = john.find(":");
		if(id != string::npos)
			john = string("B:") + john.substr(id+1, john.length()-id-1) + ":" + john.substr(0, id);
	}
	
	sprintf(cmd, fmt, start, end, charsets[charset], others, john.c_str());
	
	int pid = this->Exec(item->guid, path, cmd, MonitorThread, true, true, false);
	
	if(pid > 0){
		CLog::Log(LOG_LEVEL_NOTICE, "hashkill: Laucher OK [guid=%s, pid=%d]\n", item->guid, pid);
	}else{
		CLog::Log(LOG_LEVEL_ERROR, "hashkill: Laucher failed [guid=%s]\n", item->guid);
	}

	return pid;	
}

static float GetSpeed(const char* speed)
{
	int n = strlen(speed);
	char s[64];
	strcpy(s, speed);
	
	char f = s[n-1];
	if(f == 'M'){
		s[n-1] = 0;
		return (float)atoi(s)*1000*1000;
	} else if(f == 'K'){
		s[n-1] = 0;
		return atoi(s)*1000;
	}	
	
	return atoi(speed);
}

void *HashKill::MonitorThread(void *p)
{
	thread_param* param = (thread_param*)p;
	HashKill* hashkill = (HashKill*)param->crack;
	char guid[40];
	memcpy(guid, param->guid, sizeof(guid));
	free(param);
	time_t t0 = time(NULL), t1, t2 = t0;
	bool cracked = false;
	char buffer[2048] = {0};
	int n;
	string s;
	int idx, idx2;
	int progress, ncount;
	char temp[128];	
	char text[128] = {0};
	while(1)
	{
		n = hashkill->ReadFromLancher(guid, buffer, sizeof(buffer)-1);
		t1 = time(NULL);
		if(n == ERR_CHILDEXIT) {
			CLog::Log(LOG_LEVEL_NOMAL,"%s: Detected child exit\n", __FUNCTION__);
			break;
		}else if(n < 0){
			goto write;
		} else if(n == 0){
			continue;
		}
		buffer[n] = 0;
		s = buffer;
				
		idx = s.rfind("Progress:");//进度标志
		if(idx != string::npos){
			idx2 = s.find("\r", idx);
			if(idx2 != string::npos){
				string s2 = s.substr(idx, idx2-idx);
				int ret = sscanf(s2.c_str(), "Progress: %d%%  Speed: %*s c/s (avg: %s c/s) Cracked: %d passwords",
						&progress, temp, &ncount);
				if(ret == 3){
					CLog::Log(LOG_LEVEL_NOMAL,"%d %s %d\n", progress, temp, ncount);
					unsigned int ct = t1-t0;
					unsigned rt = (progress==0) ? 0xFFFFFFFF : (unsigned)(100.0/progress*ct)-ct;
					float speed = GetSpeed(temp);
					CLog::Log(LOG_LEVEL_NOMAL,"%d %g %d %d\n", progress, speed, ct, rt);
					hashkill->UpdateStatus(guid, progress, speed, ct, rt);
					if(hashkill->statusFunc)
							hashkill->statusFunc(guid, progress, speed, rt);
				}
			}
		}
		
		//if(idx == string::npos || ncount > 0)
		{
			idx =  s.rfind("Preimage:\n");//破解标志
			if(idx != string::npos){
				idx += strlen("Preimage:\n");
				idx2 = s.find("---\n", idx);
				if(idx2 != string::npos){
					idx2 += strlen("---\n");
					int idx3 = s.find("\n", idx2);
					if(idx2 != string::npos){
						string s2 = s.substr(idx2, idx3-idx2);
						if(sscanf(s2.c_str(), "%*s %*s %s", temp) == 1)
						{
							cracked = true;
							strcpy(text, temp);
							CLog::Log(LOG_LEVEL_NOTICE, "++++++++++ \"%s\" ++++++++++\n", text);
							goto write;
						}
					}
				}
			}
		}
		
write:	
		if(t1 - t2 >= 2 || t1 == t0)
		{
			t2 = t1;
			n = hashkill->WriteToLancher(guid, "\n", 1);
			if(n == ERR_CHILDEXIT)
			{			
				CLog::Log(LOG_LEVEL_NOMAL,"%s: Detected child exit2\n", __FUNCTION__);
				break;
			}
		}
	}
	
	if(hashkill->doneFunc)
		hashkill->doneFunc(guid, cracked, text);

	return NULL;
}
