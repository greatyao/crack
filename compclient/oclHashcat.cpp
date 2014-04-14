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
	const char* params[max_value]; 
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
/*	{algo_md4,			{"-m 900 -a 3 --increment-min=%d --increment-max=%d %s %s %s"}},*/
	{algo_md5,			{"-m 0 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_md5md5,		"-p md5md5 -b%d:%d:%s %s %s"},
	{algo_md5unix,		{"-m 500 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_mediawiki,	"-p mediawiki -b%d:%d:%s %s %s"},
	{algo_oscommerce,	{"-m 21 -a 3 --increment-min=%d --increment-max=%d %s %s %s",  "-m 0 -a 3 %s %s %s"}},
//	{algo_ipb2,			"-p ipb2 -b%d:%d:%s %s %s"},
	{algo_joomla,		{"-m 11 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_vbulletin,    "-p vbulletin -b%d:%d:%s %s %s"},
	{algo_desunix,      {"-m 1500 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_sha1,         {"-m 100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},//finished
//	{algo_sha1sha1,     "-p sha1sha1 -b%d:%d:%s %s %s"},
	{algo_sha256,       {"-m 1400 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_sha256unix,   "-p sha256unix -b%d:%d:%s %s %s"},
	{algo_sha512,       {"-m 1700 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
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
	{algo_lm,           {"-m 3000 -a 3 --increment-min=%d --incremnet-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_luks,         "-p luks -b%d:%d:%s %s %s"},
	{algo_mscash,       {"-m 1100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_mscash2,      {"-m 2100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_mssql_2000,   {"-m 131 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_mssql_2005,   {"-m 132 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_mssql_2012,   {"-m 1731 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_mysql5,       "-p mssql5 -b%d:%d:%s %s %s"},
	{algo_nsldap,       {"-m 101 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_nsldaps,      {"-m 111 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_ntlm,         {"-m 1000 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_o5logon,      "-p o5logon -b%d:%d:%s %s %s"},
//	{algo_oracle_old,   "-m 3100 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},//something wrong with oclhashcat
//	{algo_oracle11g,    "-m 112 -a 3 --increment-min=%d --increment-max=%d %s %s %s"},//wrong with hash-length inputed
//	{algo_osx_old,      "-p osx-old -b%d:%d:%s %s %s"},
	{algo_osxlion,      {"-m 1722 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
	{algo_phpbb3,       {"-m 400 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
//	{algo_pixmd5,       "-p pixmd5 -b%d:%d:%s %s %s"},
//	{algo_privkey,      "-p privkey -b%d:%d:%s %s %s"},
	{algo_ripemd160,    {"-m 6000 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 3 %s %s %s"}},
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

int oclHashcat::SupportMultiTasks()const
{
	return 0;
}

int oclHashcat::Launcher(const crack_block* item, bool gpu, unsigned short* deviceIds, int ndevices)
{
	unsigned char algo = item->algo;
	unsigned char charset = item->charset;
	unsigned short start = item->start;
	unsigned short end = item->end;
	unsigned char type = item->type;
	const char* hash = item->john;
	const char* fmt;
	unsigned short platformId = deviceIds[0] >> 8;
        unsigned short ids[16] = {0};
	char cmd[4096];
        char others[128];
	int i,j;
	struct maphashtarget a;
	
	if(!gpu)
		return ERR_INVALID_PARAM;
	switch(type){
	case bruteforce:
		for(i = 0; i < SUPPORT_HASH_NUM; i++)
		{
			if(algo == all_support_hashes[i].algo && type < max_value && type>= 0)
			{
				fmt = all_support_hashes[i].params[type];
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
	
		if(ndevices < 1 || ndevices > 16)
		{
			return ERR_INVALID_PARAM;
		}
	
		for(i = 0; i < ndevices; i++)
			ids[i] = (deviceIds[i] & 0xff) + 1;

		sprintf(others, "-d ", platformId);
		for(i = 0; i < ndevices-1; i++)
			sprintf(others, "%s%d,", others, ids[i]);
		sprintf(others, "%s%d", others, ids[i]);

		sprintf(cmd, fmt, start, end, charsets[charset], others, item->john);
		//struct maphashtarget a;
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
		for(i = 0; i < SUPPORT_HASH_NUM; i++)
		{
			if(algo == all_support_hashes[i].algo && type < max_value && type>= 0)
			{
				fmt = all_support_hashes[i].params[type];
				break;
			}
		}
		if(i == SUPPORT_HASH_NUM)
		{	
			//¿¿¿¿¿¿¿¿¿
			return ERR_NO_SUPPORT_ALGO;
		}
		if(ndevices < 1 || ndevices > 16)
		{
			return ERR_INVALID_PARAM;
		}
		for(j = 0; j < ndevices; j++)
			ids[j] = deviceIds[j] & 0xff;		
		sprintf(others, "-d ", platformId);
                for(i = 0; i < ndevices-1; i++)
                        sprintf(others, "%s%d,", others, ids[i]+1);
                sprintf(others, "%s%d", others, ids[i]+1);
		sprintf(cmd,fmt,others,item->john,"~/dic.1");
		//sprintf(cmd, fmt, start, end, charsets[charset], others, item->john);
		break;
	default:
                CLog::Log(LOG_LEVEL_NOMAL, "#######   crack type: %d  ###########\n",type);
                break;
	}

	int pid = this->Exec(item->guid, path, cmd, MonitorThread, true, true, false);
	
	if(pid > 0){
		CLog::Log(LOG_LEVEL_NOTICE, "oclHashcat: Laucher OK [guid=%s, pid=%d]\n", item->guid, pid);
	}else{
		CLog::Log(LOG_LEVEL_ERROR, "oclHashcat: Laucher failed [guid=%s]\n", item->guid);
	}

	return pid;	
}

static float GetSpeed(const char* speed)
{
	int n = strlen(speed);
	char s[64];
	strcpy(s, speed);
	char f = s[0];
	if(f == 'M'){
		return 1000*1000;
	} else if(f == 'k' || f == 'K'){
		return 1000;
	}		
	return 1;
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
	double percent=0.0,tempspeed=0.0;
	char avgspeed[128]={0};	
	char text[128]={0};
	map<string,struct maphashtarget>::iterator iter;
	iter = ocl_hashcat->MapTargetHash.find(guid);
	s_hash_with_comma = iter->second.hash;
	s_hash_with_comma.append(":");
	algo = iter->second.algo;

	while(1)
	{
		n = ocl_hashcat->ReadFromLancher(guid, buffer, sizeof(buffer)-1);
		//CLog::Log(LOG_LEVEL_NOMAL,"read[%d]\n", n);
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
		//CLog::Log(LOG_LEVEL_NOMAL,"read[%d]\n", n);
		s = buffer;
		
		if(n>0)
			CLog::Log(LOG_LEVEL_NOMAL,"%s\n", buffer);
		
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
		idx = s.rfind("Speed.GPU.#*...:");
		if(idx == string::npos)
			idx = s.rfind("Speed.GPU.#1...:");
		if(idx != string::npos){
			idx += strlen("Speed.GPU.#1...:");
			idx2 = s.find("\n",idx);
			if(idx2 != string::npos){
				string s2 = s.substr(idx,idx2-idx);
				int ret = sscanf(s2.c_str(),"%lf %s",&tempspeed,&avgspeed);
				if(ret == 2){
					idx = s.rfind("Progress.......:");
                			if(idx != string::npos){
                        			idx += strlen("Progress.......:");
									idx2 = s.find("\n",idx);
			                        if(idx2 != string::npos){
                        		    		string s2 = s.substr(idx,idx2-idx);
		                               		int ret = sscanf(s2.c_str(),"%*llu/%*llu (%lf%%)",&percent);

                		                		if(ret == 1){
                                		        		CLog::Log(LOG_LEVEL_NOMAL,"Progress is %f \n",percent);
									unsigned int ct = t1-t0;
									unsigned rt = (percent==0.0f)?0xFFFFFFFF : (unsigned)(100.0/percent*ct)-ct;
                                			       		if(ocl_hashcat->statusFunc)
                                               					 ocl_hashcat->statusFunc(guid, percent, tempspeed*GetSpeed(avgspeed), rt);
               //                        CLog::Log(LOG_LEVEL_NOMAL,"Progress updated!!!!!!!!!!!1 \n");
                                				}
                        			}
                			}
				}
			}
		}
confirm:
		idx = s.rfind("Status.........: Cracked");//¿¿¿¿
        if(idx != string::npos){
     		CLog::Log(LOG_LEVEL_ERROR,"%s: Confirming Cracked successfully\n", guid);
			cracked = true;
			//break;
		}
	
write:	
		if(t1 - t2 >= 2)
		{
			t2 = t1;
			
			n = ocl_hashcat->WriteToLancher(guid, "s", 1);
			if(n == ERR_CHILDEXIT)
			{
				CLog::Log(LOG_LEVEL_NOMAL,"%s: Detected child exit2\n", __FUNCTION__);
				break;
			}
			
		}
	}
	
	if(ocl_hashcat->doneFunc)
		ocl_hashcat->doneFunc(guid,cracked,s_result.c_str());
	iter = ocl_hashcat->MapTargetHash.find(guid);	
	printf("++++++++++++++++++++++++++++++++++++++\n");
	//ocl_hashcat->MapTargetHash.erase(iter);//TODO:ADD LOCK 
	return NULL;
}
