#include "oclHashcat.h"
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

struct maphashtarget{
	unsigned char algo;
	char hash[260];
};

struct hash_parameter{
	crack_algorithm algo;
	const char* params; 
};

static const char* charsets[] = {
	"--custom-charset1=?d",//	"num",		
	"--custom-charset1=?l",//	"lalpha",
	"--custom-charset1=?u",//"ualpha",
	"--custom-charset1=?l?u",//"alpha",
	"--custom-charset1=?l?d",//"lalphanum",
	"--custom-charset1=?u?d",//"ualphanum",
	"--custom-charset1=?u?l?d",//"alphanum",
	"--custom-charset1=?a",//"ascii",
	"",//"",
};

static struct hash_parameter all_support_hashes[] = 
{
	{algo_md4,			"-m 900 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_md5,			"-m 0 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_md5md5,		"-p md5md5 -b%d:%d:%s %s %s"},
	{algo_md5unix,		"-m 500 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_mediawiki,	"-p mediawiki -b%d:%d:%s %s %s"},
	{algo_oscommerce,	"-m 21 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_ipb2,			"-p ipb2 -b%d:%d:%s %s %s"},
	{algo_joomla,		"-m 11 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_vbulletin,    "-p vbulletin -b%d:%d:%s %s %s"},
	{algo_desunix,      "-m 1500 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_sha1,         "-m 100 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},//finished
//	{algo_sha1sha1,     "-p sha1sha1 -b%d:%d:%s %s %s"},
	{algo_sha256,       "-m 1400 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_sha256unix,   "-p sha256unix -b%d:%d:%s %s %s"},
	{algo_sha512,       "-m 1700 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_sha512unix,   "-p sha512unix -b%d:%d:%s %s %s"},
//	{algo_msoffice_old, "-p msoffice-old -b%d:%d:%s %s %s"},
//	{algo_msoffice,     "-p msoffice -b%d:%d:%s %s %s"},
//	{algo_django256,    "-p django256 -b%d:%d:%s %s %s"},
//	{algo_zip,          "-p zip -b%d:%d:%s %s %s"},
//	{algo_rar,          "-p rar -b%d:%d:%s %s %s"},
//	{algo_apr1,         "-p apr1 -b%d:%d:%s %s %s"},
//	{algo_bfunix,       "-p bfunix -b%d:%d:%s %s %s"},
//	{algo_dmg,          "-p dmg -b%d:%d:%s %s %s"},
//	{algo_drupal7,      "-p drupa17 -b%d:%d:%s %s %s"},
	{algo_lm,           "-m 3000 -a 3 --increment-min=%d --incremnet-max=%d %s %s %s"},
//	{algo_luks,         "-p luks -b%d:%d:%s %s %s"},
	{algo_mscash,       "-m 1100 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_mscash2,      "-m 2100 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_mssql_2000,   "-m 131 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_mssql_2005,   "-m 132 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_mssql_2012,   "-m 1731 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_mysql5,       "-p mssql5 -b%d:%d:%s %s %s"},
	{algo_nsldap,       "-m 101 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_nsldaps,      "-m 111 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_ntlm,         "-m 1000 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_o5logon,      "-p o5logon -b%d:%d:%s %s %s"},
//	{algo_oracle_old,   "-m 3100 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},//something wrong with oclhashcat
//	{algo_oracle11g,    "-m 112 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},//wrong with hash-length inputed
//	{algo_osx_old,      "-p osx-old -b%d:%d:%s %s %s"},
	{algo_osxlion,      "-m 1722 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
	{algo_phpbb3,       "-m 400 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_pixmd5,       "-p pixmd5 -b%d:%d:%s %s %s"},
//	{algo_privkey,      "-p privkey -b%d:%d:%s %s %s"},
	{algo_ripemd160,    "-m 6000 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_sapg,         "-p sapg -b%d:%d:%s %s %s"},
//	{algo_sl3,          "-p sl3 -b%d:%d:%s %s %s"},
//	{algo_smf,          "-p smf -b%d:%d:%s %s %s"},
//	{algo_wordpress,    "-m 500 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},
//	{algo_wpa,          "-p wpa -b%d:%d:%s %s %s"},
};

#define SUPPORT_HASH_NUM sizeof(all_support_hashes)/sizeof(all_support_hashes[0])

oclHashcat::oclHashcat(void)
{
}

oclHashcat::~oclHashcat(void)
{
}

int oclHashcat::Launcher(const crack_block* item, bool gpu, unsigned short deviceId)
{
	unsigned short platformId = deviceId >> 8;
	deviceId &= 0xff;
	unsigned char algo = item->algo;
	unsigned char charset = item->charset;
	unsigned short start = item->start;
	unsigned short end = item->end;
	unsigned char type = item->type;
	const char* hash = item->john;
	const char* fmt;
	int i;
	char cmd[4096];
	char others[128];

	if(!gpu)
		return ERR_INVALID_PARAM;
		
	switch(type){
	case bruteforce:
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
			//¿¿¿¿¿¿¿¿¿
	        return ERR_NO_SUPPORT_ALGO;
        }
		if(charset < charset_num || charset > charset_ascii)
        {
			//¿¿¿¿¿¿¿¿
			return ERR_NO_SUPPORT_CHARSET;
        }
	 	
        sprintf(others, "-d %d", deviceId+1);
		sprintf(cmd, fmt, start, end, charsets[charset], others, item->john);
		struct maphashtarget a;
		a.algo = algo;
		memcpy(a.hash,item->john,sizeof(item->john));
		if(algo==algo_mssql_2005||algo==algo_mssql_2012)
        {
			int i=0;
			char c;
			while(a.hash[i])
			{
				c=a.hash[i];
				a.hash[i]=tolower(c);
				i++;
			}
		}
		this->MapTargetHash.insert(pair<string,maphashtarget>(item->guid,a));
		break;
	case dict:
		break;
	case rule:
		break;
	default:
		CLog::Log(LOG_LEVEL_NOMAL, "#######   crack type: %d  ###########\n",type);
		break;
	}

	int pid = this->Exec(item->guid, path, cmd, MonitorThread, true, true, false);

	if(pid > 0){
		CLog::Log(LOG_LEVEL_NOMAL, "oclHashcat: [pid=%d] laucher %s\n", pid, cmd);
	}else{
		CLog::Log(LOG_LEVEL_ERROR, "oclHashcat: failed to laucher %s[%d]\n", cmd, pid);
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

void *oclHashcat::MonitorThread(void *p)
{
	thread_param* param = (thread_param*)p;
	oclHashcat* ocl_hashcat = (oclHashcat*)param->crack;
	char guid[40];
	memcpy(guid, param->guid, sizeof(guid));
	free(param);
	time_t t0 = time(NULL), t1, t2 = t0;
    bool cracked = false;	
	char buffer[2048] = {0};
	int n;
	string s,s_result,s_hash_with_comma;
	unsigned char algo;
	int idx, idx2,idx3;
	int progress, ncount;
	char avgspeed[128];	
	char text[128]={0};
	map<string,struct maphashtarget>::iterator iter;
	iter = ocl_hashcat->MapTargetHash.find(guid);
	s_hash_with_comma = iter->second.hash;
	s_hash_with_comma.append(":");
	algo = iter->second.algo;

	while(1)
	{
		n = ocl_hashcat->ReadFromLancher(guid, buffer, sizeof(buffer)-1);
		t1 = time(NULL);
		if(n == ERR_CHILDEXIT) {
			CLog::Log(LOG_LEVEL_ERROR,"%s: Detected child exit\n",__FUNCTION__);
			break;
		}else if(n < 0){
			goto write;
		} else if(n == 0){
			continue;
		}
		buffer[n] = 0;
		if(n!=0)
			CLog::Log(LOG_LEVEL_NOMAL,"read[%d] %s\n", n, buffer);
		s = buffer;
		
		if(algo==algo_mssql_2000)
			idx = s.rfind(s_hash_with_comma.substr(0,14));//mssql_2000:
		else
			idx = s.rfind(s_hash_with_comma);
		if(idx != string::npos){
			idx2 = s.find("\n",idx);
			if(idx2 != string::npos){
				string s2=s.substr(idx,idx2-idx);
				idx3 = s2.find(":",s_hash_with_comma.length()-1);// fixed here
				if(idx3>0)
				{
					s_result=s2.substr(idx3+1,idx2-idx3);
					CLog::Log(LOG_LEVEL_ERROR,"%s\n",s_result.c_str());
					goto confirm;
				}
			}                   
		}
		idx = s.rfind("Status.........: Exhausted");
		if(idx != string::npos){
			CLog::Log(LOG_LEVEL_ERROR,"Exhausted Cracking\n");
			cracked = false;
		}	
confirm:
		idx = s.rfind("Status.........: Cracked");//¿¿¿¿
        if(idx != string::npos){
     		CLog::Log(LOG_LEVEL_ERROR,"%s: Confirming Cracked successfully\n", guid);
			cracked = true;
			break;
		}
	
write:	
		if(t1 - t2 >= 2)
		{
			t2 = t1;
			/*
			n = ocl_hashcat->WriteToLancher(guid, "\n", 1);
			if(n == ERR_NO_THISTASK || n == 0)
			{
				CLog::Log(LOG_LEVEL_NOMAL,"%s: Detected child exit2\n", __FUNCTION__);
				break;
			}
			*/
		}
	}
	
	if(ocl_hashcat->doneFunc)
		ocl_hashcat->doneFunc(guid,cracked,s_result.c_str());
	iter = ocl_hashcat->MapTargetHash.find(guid);	
	ocl_hashcat->MapTargetHash.erase(iter);//TODO:ADD LOCK 
	return NULL;
}
