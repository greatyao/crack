#include "HashKill.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <float.h> 

using std::string;

#ifdef __CYGWIN__
const char* path = "hashkill.exe";
#else
const char* path = "/usr/local/bin/hashkill";
#endif

struct hash_parameter{
	crack_algorithm algo;
	const char* params; 
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
	{algo_md4,			"-p md4 -b%d:%d:%s %s %s"},
	{algo_md5,			"-p md5 -b%d:%d:%s %s %s"},
	{algo_md5md5,		"-p md5md5 -b%d:%d:%s %s %s"},
	{algo_md5unix,		"-p md5unix -b%d:%d:%s %s %s"},
	{algo_mediawiki,	"-p mediawiki -b%d:%d:%s %s %s"},
	{algo_oscommerce,	"-p oscommerce -b%d:%d:%s %s %s"},
	{algo_ipb2,			"-p ipb2 -b%d:%d:%s %s %s"},
	{algo_joomla,		"-p joomla -b%d:%d:%s %s %s"},
	{algo_vbulletin,    "-p vbulletin -b%d:%d:%s %s %s"},
	{algo_desunix,      "-p desunix -b%d:%d:%s %s %s"},
	{algo_sha1,         "-p sha1 -b%d:%d:%s %s %s"},
	{algo_sha1sha1,     "-p sha1sha1 -b%d:%d:%s %s %s"},
	{algo_sha256,       "-p sha256 -b%d:%d:%s %s %s"},
	{algo_sha256unix,   "-p sha256unix -b%d:%d:%s %s %s"},
	{algo_sha512,       "-p sha512 -b%d:%d:%s %s %s"},
	{algo_sha512unix,   "-p sha512unix -b%d:%d:%s %s %s"},
	{algo_msoffice_old, "-p msoffice-old -b%d:%d:%s %s %s"},
	{algo_msoffice,     "-p msoffice -b%d:%d:%s %s %s"},
	{algo_django256,    "-p django256 -b%d:%d:%s %s %s"},
	{algo_zip,          "-p zip -b%d:%d:%s %s %s"},
	{algo_rar,          "-p rar -b%d:%d:%s %s %s"},
	{algo_apr1,         "-p apr1 -b%d:%d:%s %s %s"},
	{algo_bfunix,       "-p bfunix -b%d:%d:%s %s %s"},
	{algo_dmg,          "-p dmg -b%d:%d:%s %s %s"},
	{algo_drupal7,      "-p drupa17 -b%d:%d:%s %s %s"},
	{algo_lm,           "-p lm -b%d:%d:%s %s %s"},
	{algo_luks,         "-p luks -b%d:%d:%s %s %s"},
	{algo_mscash,       "-p mscash -b%d:%d:%s %s %s"},
	{algo_mscash2,      "-p mscash2 -b%d:%d:%s %s %s"},
	{algo_mssql_2000,   "-p mssql-2000 -b%d:%d:%s %s %s"},
	{algo_mssql_2005,   "-p mssql-2005 -b%d:%d:%s %s %s"},
	{algo_mssql_2012,   "-p mssql-2012 -b%d:%d:%s %s %s"},
	{algo_mysql5,       "-p mssql5 -b%d:%d:%s %s %s"},
	{algo_nsldap,       "-p nsldap -b%d:%d:%s %s %s"},
	{algo_nsldaps,      "-p nsldaps -b%d:%d:%s %s %s"},
	{algo_ntlm,         "-p ntlm -b%d:%d:%s %s %s"},
	{algo_o5logon,      "-p o5logon -b%d:%d:%s %s %s"},
	{algo_oracle_old,   "-p oracle-old -b%d:%d:%s %s %s"},
	{algo_oracle11g,    "-p oracle11g -b%d:%d:%s %s %s"},
	{algo_osx_old,      "-p osx-old -b%d:%d:%s %s %s"},
	{algo_osxlion,      "-p osxlion -b%d:%d:%s %s %s"},
	{algo_phpbb3,       "-p phpbb3 -b%d:%d:%s %s %s"},
	{algo_pixmd5,       "-p pixmd5 -b%d:%d:%s %s %s"},
	{algo_privkey,      "-p privkey -b%d:%d:%s %s %s"},
	{algo_ripemd160,    "-p ripemd160 -b%d:%d:%s %s %s"},
	{algo_sapg,         "-p sapg -b%d:%d:%s %s %s"},
	{algo_sl3,          "-p sl3 -b%d:%d:%s %s %s"},
	{algo_smf,          "-p smf -b%d:%d:%s %s %s"},
	{algo_wordpress,    "-p wordpress -b%d:%d:%s %s %s"},
	{algo_wpa,          "-p wpa -b%d:%d:%s %s %s"},
};

#define SUPPORT_HASH_NUM sizeof(all_support_hashes)/sizeof(all_support_hashes[0])

HashKill::HashKill(void)
{
}

HashKill::~HashKill(void)
{
}

int HashKill::Launcher(const crack_block* item, bool gpu, unsigned short deviceId)
{
	unsigned short platformId = deviceId >> 8;
	deviceId &= 0xff;
	crack_algorithm algo = item->algo;
	crack_charset charset = item->charset;
	unsigned short start = item->start;
	unsigned short end = item->end;
	const char* hash = item->john;
	const char* fmt;
	int i;

	for(i = 0; i < SUPPORT_HASH_NUM; i++)
	{
		if(algo == all_support_hashes[i].algo)
		{
			fmt = all_support_hashes[i].params;
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

	sprintf(cmd, fmt, start, end, charsets[charset], others, item->john);
	
	int pid = this->Exec(item->guid, path, cmd, MonitorThread);
	
	if(pid > 0){
		CLog::Log(LOG_LEVEL_NOMAL, "hashkill: [pid=%d] laucher %s\n", pid, cmd);
	}else{
		CLog::Log(LOG_LEVEL_ERROR, "hashkill: failed to laucher %s[%d]\n", cmd, pid);
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
	} else if(f == 'k'){
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
	time_t t0 = time(NULL);
	
	char buffer[2048] = {0};
	int n;
	int cumm = 0;
	string s;
	int idx, idx2;
	int progress, ncount;
	char avgspeed[128];	
	char text[32];
	while(1)
	{
		n = hashkill->ReadFromLancher(guid, buffer, sizeof(buffer)-1);
		if(n == 0) {
			printf("[done]\n");
			break;
		}else if(n < 0){
			cumm ++;
			if(cumm == 5) goto write;
			sleep(1);
			continue;
		} 
		buffer[n] = 0;
		//CLog::Log(LOG_LEVEL_NOMAL,"read[%d]: %s", n, buffer);
		s = buffer;
				
		idx = s.rfind("Progress:");//进度标志
		if(idx != string::npos){
			idx2 = s.find("\r", idx);
			if(idx2 != string::npos){
				string s2 = s.substr(idx, idx2-idx);
				int ret = sscanf(s2.c_str(), "Progress: %d%%  Speed: %*s c/s (avg: %s c/s) Cracked: %d passwords",
						&progress, avgspeed, &ncount);
				if(ret == 3){
					CLog::Log(LOG_LEVEL_NOMAL,"%d %s %d\n", progress, avgspeed, ncount);
					unsigned int ct = time(NULL)-t0;
					unsigned rt = (progress==0) ? 0xFFFFFFFF : 100/progress*ct;
					float speed = GetSpeed(avgspeed);
					CLog::Log(LOG_LEVEL_NOMAL,"%d %g %d %d\n", progress, speed, ct, rt);
					hashkill->UpdateStatus(guid, progress, speed, ct, rt);
					if(hashkill->statusFunc)
							hashkill->statusFunc(guid, progress, speed, rt);
				}
			}
		}
		
		if(ncount > 0){
			idx =  s.rfind("Preimage:\n");//破解标志
			if(idx != string::npos){
				idx += strlen("Preimage:\n");
				idx2 = s.find("----------\n", idx);
				if(idx2 != string::npos){
					idx2 += strlen("----------\n");
					int idx3 = s.find("\n", idx2);
					if(idx2 != string::npos){
						string s2 = s.substr(idx2, idx3-idx2);
						CLog::Log(LOG_LEVEL_NOMAL, "%d [%s]\n", s2.length(), s2.c_str());
						sscanf(s2.c_str(), "%*s %*s %s", text);
						CLog::Log(LOG_LEVEL_NOMAL, "[%s]\n", text);
						if(hashkill->doneFunc)
							hashkill->doneFunc(guid, true, text);
					}
				}
			}
		}
		
		idx = s.rfind("Bye bye");//结束标志
		if(idx != string::npos){
			continue;
		}
		
write:	
		cumm = 0;
		n = hashkill->WriteToLancher(guid, "\n", 1);
		//CLog::Log(LOG_LEVEL_NOMAL, "write %d\n", n);
		sleep(2);
	}
	return NULL;
}

int HashKill::Kill(const char* guid)
{
	char ctrl_c = char(03);
	return this->WriteToLancher(guid, &ctrl_c, 1);
}