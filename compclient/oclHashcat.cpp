#include "oclHashcat.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"
#include "CrackManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <float.h> 
#include <unistd.h>
#include <ctype.h>
#include <algorithm>

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
	{algo_md4,		{"-m 900 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 900 -a 0 %s %s %s",NULL,"-m 900 -a 3 %s %s %s"}},
	{algo_md5,		{"-m 0 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 0 -a 0 %s %s %s",NULL,"-m 0 -a 3 %s %s %s"}},
	{algo_md5md5,		{"-m 2600 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 2600 -a 0 %s %s %s",NULL,"-m 2600 -a 3 %s %s %s"}},
	{algo_md5unix,		{"-m 500 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 500 -a 0 %s %s %s",NULL,"-m 500 -a 3 %s %s %s"}},
//	{algo_mediawiki,	"-p mediawiki -b%d:%d:%s %s %s"},
	{algo_oscommerce,	{"-m 21 -a 3 --increment-min=%d --increment-max=%d %s %s %s",  "-m 21 -a 0 %s %s %s",NULL,"-m 21 -a 3 %s %s %s"}},
	{algo_ipb2,		{"-m 2811 -a 3 --increment-min=%d --increment-max=%d %s %s %s",  "-m 2811 -a 0 %s %s %s",NULL,"-m 2811 -a 3 %s %s %s"}},
	{algo_joomla,		{"-m 11 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 11 -a 0 %s %s %s",NULL,"-m 11 -a 3 %s %s %s"}},
	{algo_vbulletin,    	{"-m 2611 -a 3 --increment-min=%d --increment-max=%d %s %s %s",  "-m 2611 -a 0 %s %s %s",NULL,"-m 2611 -a 3 %s %s %s"}},
	{algo_desunix,      {"-m 1500 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1500 -a 0 %s %s %s",NULL,"-m 1500 -a 3 %s %s %s"}},
	{algo_sha1,         {"-m 100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 100 -a 0 %s %s %s",NULL,"-m 100 -a 3 %s %s %s"}},//finished
//	{algo_sha1sha1,     "-p sha1sha1 -b%d:%d:%s %s %s"},
	{algo_sha256,       {"-m 1400 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1400 -a 0 %s %s %s",NULL,"-m 1400 -a 3 %s %s %s"}},
	{algo_sha256unix,   {"-m 7400 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 7400 -a 0 %s %s %s",NULL,"-m 7400 -a 3 %s %s %s"}},
	{algo_sha512,       {"-m 1700 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1700 -a 0 %s %s %s",NULL,"-m 1700 -a 3 %s %s %s"}},
	{algo_sha512unix,   {"-m 1800 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1800 -a 0 %s %s %s",NULL,"-m 1800 -a 3 %s %s %s"}},
//	{algo_msoffice_old, "-p msoffice-old -b%d:%d:%s %s %s"},
//	{algo_msoffice,     "-p msoffice -b%d:%d:%s %s %s"},
//	{algo_django256,    "-p django256 -b%d:%d:%s %s %s"},
//	{algo_zip,          "-p zip -b%d:%d:%s %s %s"},
//	{algo_rar,          "-p rar -b%d:%d:%s %s %s"},
	{algo_apr1,         {"-m 1600 -a 3 --increment-min=%d --incremnet-max=%d %s %s %s", "-m 1600 -a 0 %s %s %s",NULL,"-m 1600 -a 3 %s %s %s"}},// something wrong
	{algo_bfunix,      {"-m 3200 -a 3 --increment-min=%d --incremnet-max=%d %s %s %s", "-m 3200 -a 0 %s %s %s",NULL,"-m 3200 -a 3 %s %s %s"}},
//	{algo_dmg,          "-p dmg -b%d:%d:%s %s %s"},
//	{algo_drupal7,      "-p drupa17 -b%d:%d:%s %s %s"},
	{algo_lm,           {"-m 3000 -a 3 --increment-min=%d --incremnet-max=%d %s %s %s", "-m 3000 -a 0 %s %s %s",NULL,"-m 3000 -a 3 %s %s %s"}},
//	{algo_luks,         "-p luks -b%d:%d:%s %s %s"},
	{algo_mscash,       {"-m 1100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1100 -a 0 %s %s %s",NULL,"-m 1100 -a 3 %s %s %s"}},
	{algo_mscash2,      {"-m 2100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 2100 -a 0 %s %s %s",NULL,"-m 2100 -a 3 %s %s %s"}},
	{algo_mssql_2000,   {"-m 131 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 131 -a 0 %s %s %s",NULL,"-m 131 -a 3 %s %s %s"}},
	{algo_mssql_2005,   {"-m 132 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 132 -a 0 %s %s %s",NULL,"-m 132 -a 3 %s %s %s"}},
	{algo_mssql_2012,   {"-m 1731 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1731 -a 0 %s %s %s",NULL,"-m 1731 -a 3 %s %s %s"}},
	{algo_mysql5,       {"-m 300 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 300 -a 0 %s %s %s",NULL,"-m 300 -a 3 %s %s %s"}},
	{algo_nsldap,       {"-m 101 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 101 -a 0 %s %s %s",NULL,"-m 101 -a 3 %s %s %s"}},
	{algo_nsldaps,      {"-m 111 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 111 -a 0 %s %s %s",NULL,"-m 111 -a 3 %s %s %s"}},
	{algo_ntlm,         {"-m 1000 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1000 -a 0 %s %s %s",NULL,"-m 1000 -a 3 %s %s %s"}},
//	{algo_o5logon,      "-p o5logon -b%d:%d:%s %s %s"},
	{algo_oracle_old,   {"-m 3100 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 3100 -a 0 %s %s %s",NULL,"-m 3100 -a 3 %s %s %s"}},//something wrong with oclhashcat
//	{algo_oracle11g,    {"-m 112 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 112 -a 3 %s %s %s",NULL,"-m 112 -a 3 %s %s %s"}},//wrong with hash-length inputed
	{algo_osx_old,      {"-m 122 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 122 -a 0 %s %s %s",NULL,"-m 122 -a 3 %s %s %s"}},
	{algo_osxlion,      {"-m 1722 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 1722 -a 0 %s %s %s",NULL,"-m 1722 -a 3 %s %s %s"}},
	{algo_phpbb3,       {"-m 400 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 400 -a 0 %s %s %s",NULL,"-m 400 -a 3 %s %s %s"}},
//	{algo_pixmd5,       "-p pixmd5 -b%d:%d:%s %s %s"},
//	{algo_privkey,      "-p privkey -b%d:%d:%s %s %s"},
	{algo_ripemd160,    {"-m 6000 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 6000 -a 0 %s %s %s",NULL,"-m 6000 -a 3 %s %s %s"}},
//	{algo_sapg,         "-p sapg -b%d:%d:%s %s %s"},
//	{algo_sl3,          "-p sl3 -b%d:%d:%s %s %s"},
//	{algo_smf,          "-p smf -b%d:%d:%s %s %s"},
	{algo_wordpress,    {"-m 500 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 500 -a 0 %s %s %s",NULL,"-m 500 -a 3 %s %s %s"}},
	{algo_wpa,          {"-m 2500 -a 3 --increment-min=%d --increment-max=%d %s %s %s", "-m 2500 -a 0 %s %s %s",NULL,"-m 2500 -a 3 %s %s %s"}},
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
	char local_mask[1024]={0};
	char local_mask2[1024]={0};
	char charset_mask[1024]={0};
	char cmd[4096];
	string john = item->john;
    char others[128];
	int i,j;
	struct maphashtarget a;
	
	if(!gpu)
		return ERR_INVALID_PARAM;
		
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
		//靠靠靠靠?
		return ERR_NO_SUPPORT_ALGO;
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
	if(algo == algo_mysql5)
	{
		int id = john.find("*");
		if(id!=string::npos)
			john = john.substr(id+1,john.length()-1);
	}
	switch(type){
	case bruteforce:
		if(algo==algo_apr1||algo==algo_bfunix)
			return ERR_NO_SUPPORT_ALGO;
		if(charset < charset_num || charset > charset_ascii)
		{
			//靠靠靠靠
			return ERR_NO_SUPPORT_CHARSET;
		}
		sprintf(cmd, fmt, start, end, charsets[charset], others, john.c_str());
		//printf("%s",cmd);
		break;
	case dict:
	{
		char dict_name[512];
		bool f = CrackManager::Get().GetDict(item->dict_idx,dict_name,sizeof(dict_name),NULL,NULL);
		if(!f) return ERR_NO_DICT;
		sprintf(cmd,fmt,others,john.c_str(),dict_name);			
		break;
	}
	case mask:
		if(item->maskLength>18||item->maskLength<1)
			return ERR_INVALID_PARAM;
		if(item->flag==0)
		{
			for(unsigned short i=0;i < item->maskLength;i++)
			{
				if((int)item->masks[i] != -1)
					sprintf(local_mask,"%s%c",local_mask,item->masks[i]);
				else if(item->charset == charset_num)
					sprintf(local_mask,"%s%s",local_mask,"?d");
				else if(item->charset == charset_lalpha)
					sprintf(local_mask,"%s%s",local_mask,"?l");
				else if(item->charset == charset_ualpha)
					sprintf(local_mask,"%s%s",local_mask,"?u");
				else if(item->charset == charset_lalphanum)
					sprintf(local_mask,"%s%s",local_mask,"?1");
				else if(item->charset == charset_ualphanum)
					sprintf(local_mask,"%s%s",local_mask,"?1");
				else if(item->charset == charset_alphanum)
					sprintf(local_mask,"%s%s",local_mask,"?1");
                else
					sprintf(local_mask,"%s%s",local_mask,"?a");
			}
			if(item->charset==charset_num)
				sprintf(local_mask2,"%s",local_mask);
			else if(item->charset==charset_lalpha)
				sprintf(local_mask2,"%s",local_mask);
			else if(item->charset==charset_ualpha)
				sprintf(local_mask2,"%s",local_mask);
			else if(item->charset==charset_lalphanum)
				sprintf(local_mask2,"--custom-charset1=?l?d %s", local_mask);
			else if(item->charset==charset_ualphanum)
				sprintf(local_mask2,"--custom-charset1=?u?d %s", local_mask);
			else if(item->charset==charset_alphanum)
				sprintf(local_mask2,"--custom-charset1=?u?l?d %s", local_mask);
			else
				sprintf(local_mask2,"%s", local_mask);
			sprintf(cmd,fmt,others,john.c_str(),local_mask2);
		}
		else if(item->flag==1){
			//printf("%s\n",item->masks1);
			//printf("%s\n",item->masks2);
			unsigned int it = 0;
			while((item->masks1[it])==(item->masks2[it])){it++;}
			//printf("%d\n",it);
			//printf("mask length %d\n",item->maskLength);
			unsigned int j=0,k=0;
			while((item->masks1[it])!=(crack_charsets[item->charset][j])){j++;}
			while((item->masks2[it])!=(crack_charsets[item->charset][k])){k++;}
			//printf("charset index:%d %d\n",j,k);
			//sprintf(charset_mask,"?2");
			//printf("charset_mask %s \n",charset_mask);
			for(unsigned int m=j;m<=k;m++)
				sprintf(charset_mask,"%s%c",charset_mask,crack_charsets[item->charset][m]);
			//printf("charset_mask %s \n",charset_mask);
			for(j=item->maskLength-1;j>it;j--)
			{
				if(item->charset==charset_num){
					sprintf(local_mask,"%s%s",local_mask,"?d");}
				else if(item->charset==charset_lalpha){
					sprintf(local_mask,"%s%s",local_mask,"?l");}
				else if(item->charset==charset_ualpha){
					sprintf(local_mask,"%s%s",local_mask,"?u");}
				else if(item->charset==charset_lalphanum){
					sprintf(local_mask,"%s%s",local_mask,"?1");}
				else if(item->charset==charset_ualphanum){
					sprintf(local_mask,"%s%s",local_mask,"?1");}
				else if(item->charset==charset_alphanum){
					sprintf(local_mask,"%s%s",local_mask,"?1");}
				else
					sprintf(local_mask,"%s%s",local_mask,"?a");
			}
			sprintf(local_mask2,"?2%s",local_mask);
			sprintf(local_mask,"");
			for(i=0;i<it;i++)
				sprintf(local_mask,"%s%c",local_mask,(char)item->masks1[i]);
			sprintf(local_mask,"%s%s",local_mask,local_mask2);
			//printf("local_mask2 %s \n",local_mask2);
			//printf("local_mask %s \n",local_mask);
		}
		if(item->charset==charset_num)
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?d",local_mask);
		else if(item->charset==charset_lalpha)
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?l",local_mask);
		else if(item->charset==charset_ualpha)
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?u",local_mask);
		else if(item->charset==charset_lalphanum)
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?l?d",local_mask);
		else if(item->charset==charset_ualphanum)
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?u?d",local_mask);
		else if(item->charset==charset_alphanum)
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?u?l?d",local_mask);
		else
			sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?a",local_mask);
		//printf("%s\n",charset_mask);	
		sprintf(cmd,fmt,others,john.c_str(),local_mask2);
		break;
	default:
		CLog::Log(LOG_LEVEL_NOMAL, "#######   crack type: %d  ###########\n",type);
        break;sprintf(local_mask2,"--custom-charset2=%s --custom-charset1=%s %s",charset_mask,"?1",local_mask);
	}
	
	a.algo = algo;
	if(algo==algo_mssql_2005||algo==algo_mssql_2012||algo==algo_osx_old)
		std::transform(john.begin(), john.end(), john.begin(), ::tolower);
	memcpy(a.hash,john.c_str(),john.length());
	this->MapTargetHash.insert(pair<string,maphashtarget>(item->guid,a));
	
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
	map<string,struct maphashtarget>::iterator iter;
	iter = ocl_hashcat->MapTargetHash.find(guid);
	if(iter->second.algo==algo_wpa)
		s_hash_with_comma = "";	
	else{
		s_hash_with_comma = iter->second.hash;
		s_hash_with_comma.append(":");
	}
	algo = iter->second.algo;
	string lastS="";
	bool killed = false;
	bool is_wpa_set = iter->second.algo==algo_wpa;

	while(1)
	{
		n = ocl_hashcat->ReadFromLancher(guid, buffer, sizeof(buffer)-1);
		//CLog::Log(LOG_LEVEL_NOMAL,"read[%d]\n", n);
		t1 = time(NULL);
		if(n == ERR_CHILDEXIT) {
			CLog::Log(LOG_LEVEL_ERROR,"%s: Detected child exit\n",__FUNCTION__);
			break;
		} else if(n == ERR_NO_THISTASK){
			killed = true;
			break;
		} else if(n < 0){
			goto write;
		} else if(n == 0){
			continue;
		}
		buffer[n] = 0;
		s = lastS + buffer;
		lastS= buffer;
		
		//if(n>0)
		//	CLog::Log(LOG_LEVEL_NOMAL,"%s\n", buffer);
		if(is_wpa_set){
			idx = s.rfind("Hash.Target....:");
			if(idx != string::npos){
				idx += strlen("Hash.Target....:");
				idx2 = s.find("\n",idx);
				if(idx2 != string::npos){
					string s4=s.substr(idx,idx2-idx);
					char buf[256];
					if(sscanf(s4.c_str(), "%s", buf) == 1){
						s_hash_with_comma = buf;
						s_hash_with_comma.append(":");
						is_wpa_set = false;
					} 
				}
			}
		}

		if(is_wpa_set) goto speed0;
		
		if(algo==algo_mssql_2000)
			idx = s.rfind(s_hash_with_comma.substr(0,14));//mssql_2000:
		else
			idx = s.rfind(s_hash_with_comma);
		if(idx != string::npos){
		//	CLog::Log(LOG_LEVEL_ERROR,"###### %s\n",s.c_str());
			idx2 = s.find("\n",idx);
			if(idx2 != string::npos){
				string s2=s.substr(idx,idx2-idx);
		//		CLog::Log(LOG_LEVEL_ERROR,"@@@@@@ %s\n",s2.c_str());
				idx3 = s2.find(":",s_hash_with_comma.length()-1);// fixed here
				if(idx3>0)
				{
					s_result=s2.substr(idx3+1,idx2-idx3);
					CLog::Log(LOG_LEVEL_ERROR,"%s\n",s_result.c_str());
					goto confirm;
				}
			}                   
		}
		
speed0:
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
   							}
						}
					}
				}
			}
		}
confirm:
		idx = s.rfind("Status.........: Cracked");//靠靠
        if(idx != string::npos){
     		CLog::Log(LOG_LEVEL_ERROR,"%s: Confirming Cracked successfully\n", guid);
			cracked = true;
		}
	
write:	
		if(t1 - t2 >= 2 || is_wpa_set)
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
		ocl_hashcat->doneFunc(guid,cracked,s_result.c_str(), !killed);
	iter = ocl_hashcat->MapTargetHash.find(guid);	
	printf("++++++++++++++++++++++++++++++++++++++\n");
	//ocl_hashcat->MapTargetHash.erase(iter);//TODO:ADD LOCK 
	return NULL;
}
