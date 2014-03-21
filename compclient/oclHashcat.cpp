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

#ifdef __CYGWIN__
const static char* path = "hashkill.exe";
#else
const static char* path = "/home/gputest/oclHashcat-1.01/oclHashcat64.bin";
#endif

struct hash_parameter{
	crack_algorithm algo;
	const char* params; 
};

static const char* charsets[] = {
	"num",			 //¿¿¿crack_charset¿¿charset_num
	"lalpha",		//¿¿¿¿
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
//	{algo_md5,			"-p md5 -b%d:%d:%s %s %s"},
	{algo_md5,			"-m 0 -a 3 --increment-min=%d --increment-max=%d %s %s"},
	{algo_md5md5,		"-p md5md5 -b%d:%d:%s %s %s"},
	{algo_md5unix,		"-p md5unix -b%d:%d:%s %s %s"},
	{algo_mediawiki,	"-p mediawiki -b%d:%d:%s %s %s"},
	{algo_oscommerce,	"-p oscommerce -b%d:%d:%s %s %s"},
	{algo_ipb2,			"-p ipb2 -b%d:%d:%s %s %s"},
	{algo_joomla,		"-p joomla -b%d:%d:%s %s %s"},
	{algo_vbulletin,    "-p vbulletin -b%d:%d:%s %s %s"},
	{algo_desunix,      "-p desunix -b%d:%d:%s %s %s"},
	{algo_sha1,         "-m 100 -a 3 --increment-min=%d --increment-max=%d %s %s"},//finished
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
		//¿¿¿¿¿¿¿¿¿
		return ERR_NO_SUPPORT_ALGO;
	}

	if(charset < charset_num || charset > charset_ascii)
	{
		//¿¿¿¿¿¿¿¿
		return ERR_NO_SUPPORT_CHARSET;
	}

	char cmd[4096];
	char others[128];
	if(!gpu){
		sprintf(others, "-c");
		return ERR_INVALID_PARAM;
	}
	else
		sprintf(others, "-d %d", deviceId+2);

// sprintf(cmd, fmt, start, end, charsets[charset], others, item->john);
	sprintf(cmd, fmt, start, end, others, item->john);
 	//this->MapTargetHash[item->guid] = "acd";
	this->MapTargetHash[item->guid]=item->john;
	int pid = this->Exec(item->guid, path, cmd, MonitorThread);
	
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
	string s,s_result;
	int idx, idx2,idx3;
	int progress, ncount;
	char avgspeed[128];	
	char text[128]={0};

	while(1)
	{
		n = ocl_hashcat->ReadFromLancher(guid, buffer, sizeof(buffer)-1);
		if(n == 0) {
			CLog::Log(LOG_LEVEL_ERROR,"%s: Detected child exit\n",__FUNCTION__);
			//exit(0);
			break;
		}else if(n < 0){
			goto write;
		} 
		buffer[n] = 0;
		//CLog::Log(LOG_LEVEL_NOMAL,"read[%d] %s\n", n, buffer);
		s = buffer;
				
//	CLog::Log(LOG_LEVEL_ERROR,"%s",ocl_hashcat->crack);
		//ocl_hashcat->MapTargetHash<string,string>::iterator iter;
		//map<string,string>::iterator iter;
		// ocl_hashcat->MapTargetHash.begin();
		for(map<string,string>::iterator iter = ocl_hashcat->MapTargetHash.begin();iter!=ocl_hashcat->MapTargetHash.end();iter++)
		{
			//TODO:Check size equals 1
			//printf("%s \n",iter->first.c_str(),iter->second.c_str());
			string s_hash_with_comma = iter->second;
			s_hash_with_comma.append(":");
			//printf("%s\n",s_hash_with_comma.c_str());
			idx = s.rfind(s_hash_with_comma);
			if(idx != string::npos){
				//printf("%d\n",idx);
			//	CLog::Log(LOG_LEVEL_ERROR,"find hash\n");
			//	for(int jj = idx; jj<=idx+40; jj++)
			//		printf("%d:%c\n",jj,  s[jj]);
				idx2 = s.find("\n",idx);
				//printf("%d\n",idx2);
			//	idx3 = s.rfind("\n",idx);
				if(idx2 != string::npos){
					printf("find idx2\n");
					string s2=s.substr(idx,idx2-idx);
					idx3 = s2.find(":");
//				printf("%d\n",idx3);
					s_result=s2.substr(idx3+1,idx2-idx3);
					CLog::Log(LOG_LEVEL_ERROR,"%s\n",s_result.c_str());
					goto confirm;
				}                   
			}
			idx = s.rfind("Status.........: Exhausted");
			if(idx != string::npos){
				CLog::Log(LOG_LEVEL_ERROR,"Exhausted Cracking\n");
				cracked = false;
			}	
		}
confirm:
	idx = s.rfind("Status.........: Cracked");//¿¿¿¿
        if(idx != string::npos){
     		CLog::Log(LOG_LEVEL_ERROR,"Confirming Cracked successfully\n");
		cracked = true;
	}
	
write:	
		 if(t1 - t2 >= 2)
                {
                        t2 = t1;
                      //n = ocl_hashcat->WriteToLancher(guid, "\n", 1);
                       /* if(n == ERR_NO_THISTASK || n == 0)
                        {
                                CLog::Log(LOG_LEVEL_NOMAL,"%s: Detected child exit2\n", __FUNCTION__);
                                break;
                        }*/
                }
	}
	if(ocl_hashcat->doneFunc)
		ocl_hashcat->doneFunc(guid,cracked,s_result.c_str());
	printf("uploaded result!____________________________________\n");

	return NULL;
}
