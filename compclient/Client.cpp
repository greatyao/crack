/* Client.cpp
 *
 * Client for connection with server
 * Copyright (C) 2014 TRIMPS
 *
 * Created By YAO Wei at  03/19/2014
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/utsname.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <zlib.h>
#include <fcntl.h>

#include "Client.h"
#include "CrackManager.h"
#include "resourceslotpool.h"
#include "err.h"
#include "CLog.h"
#include "macros.h"
#include "algorithm_types.h"


static struct login_info linfo = {0};
static unsigned char pack_flag[5] = {'G', '&', 'C', 'P', 'U'};
static const int HDR_SIZE = 13;

class Lock{
public:
	Lock(pthread_mutex_t* mutex):mutex(mutex)
	{
		pthread_mutex_lock(mutex);
	}
	~Lock()
	{
		pthread_mutex_unlock(mutex);
	}
private:
	pthread_mutex_t* mutex;
};

Client& Client::Get()
{
	static Client client;
	return client;
}

Client::Client()
{
	sck = 0;
	connected = false;
	stop = false;
	tid = 0;
	fetch = true;
	pthread_mutex_init(&mutex, NULL);
}

Client::~Client()
{
	Destory();
}

void Client::Destory()
{
	if(tid)
	{
		stop = true;
		pthread_join(tid, NULL);
		tid = 0;
		CLog::Log(LOG_LEVEL_NOMAL, "Client: Succeed to exit monitor thread\n");
	}
}


void* Client::MonitorThread(void* p)
{
	Client* client = (Client*)p;
	int ret;
	char buf[1024*4];
		
	sleep(5);

	while(1)
	{
		while(client->connected != 2)
		{
			if(client->stop) return NULL;
			
			CLog::Log(LOG_LEVEL_NOMAL, "Client: reconnect to server\n");
			ret = client->Connect(client->ip, client->port);
				
			sleep(10);
		}
		CLog::Log(LOG_LEVEL_NOMAL, "------------------------------\n");
			
		time_t t0 = time(NULL);
		client->fetch = true;
		while(1)
		{
			if(client->stop) return NULL;

			sleep(10);
			
			Lock lk(&client->mutex);
			unsigned char cmd = CMD_HEARTBEAT;
			int n = client->Write(cmd, NULL, 0);
			if(n == ERR_CONNECTIONLOST) 
				break;
			
			//后续考虑心跳包里面是否有结束某个workitem的解密工作
			short status;
			int m = client->Read(&cmd, &status, buf, sizeof(buf));
			
			if(cmd == CMD_COMP_HEARTBEAT)
			{
				keeplive_compclient* ka = (keeplive_compclient*)buf;
				CLog::Log(LOG_LEVEL_NOMAL, "Client: Special Hearbreak cmd %d [%d,%d]\n", cmd, ka->tasks, ka->blocks);
				client->fetch = (ka->tasks > 0);
				
				for(int i = 0; i < ka->blocks; i++)
				{
					CLog::Log(LOG_LEVEL_NOMAL, "Client: Stop workitem [guid=%s]\n", ka->guids[i]);
					CrackManager::Get().StopCrack(ka->guids[i]);
				}
			}
			else
			{
				CLog::Log(LOG_LEVEL_NOMAL, "Client: Normal Hearbreak cmd %d %d\n", cmd, m);
				client->fetch = true;
			}
			
			time_t t1 = time(NULL);
			if(t1 - t0 >= 600)
			{	
				t0 = t1;
				client->fetch = true;
			}
		}
	}
	
	return NULL;
}

int Client::Connect(const char* ip, unsigned short port)
{
	if(connected != 0)
		return 0;
	
	connected = 1;
	strncpy(this->ip, ip, 16);
	this->port = port;
	
	if(tid == 0)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&tid, &attr, MonitorThread, (void *)this);
	}
	
	sck = socket(AF_INET, SOCK_STREAM, 0);
	if(sck < 0)
	{
		CLog::Log(LOG_LEVEL_WARNING, "Client: failed to create socket");
		connected = 0;
		close(sck);
		return ERR_INTERNALCLIENT;
	}
	
	struct sockaddr_in s_add;
	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = inet_addr(ip);
	s_add.sin_port = htons(port);
	CLog::Log(LOG_LEVEL_NOMAL, "Client: connecting to %s:%d\n", ip, port);

	if(connect(sck,(struct sockaddr *)&s_add, sizeof(struct sockaddr)))
	{
		CLog::Log(LOG_LEVEL_WARNING, "Client: failed to connect to %s:%d\n", ip, port);
		connected = 0;
		close(sck);
		return ERR_INTERNALCLIENT;
	}
	
	connected = 2;
	
	//int flag = fcntl(sck, F_GETFL, 0);
	//fcntl(sck, F_SETFL, flag|O_NONBLOCK);
	//struct timeval timeout = {3, 0};
	//setsockopt(sck, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	
	unsigned char cmd = CMD_LOGIN;
	if(linfo.m_osinfo[0] == 0)
	{
		FILE* f = fopen("/etc/issue", "r");
		if(f)
		{
			char line[256];
			if(fgets(line, sizeof(line), f))
			{
				char* p = strstr(line, "\\n");
				if(p) *p = 0;
				strncpy(linfo.m_osinfo, line, sizeof(linfo.m_osinfo));
			}
			fclose(f);
		}		
		
		struct utsname uts;
		if(uname(&uts) >= 0)
		{
			if(linfo.m_osinfo[0] == 0)
			snprintf(linfo.m_osinfo, sizeof(linfo.m_osinfo), "%s %s ", uts.sysname, uts.release);
			strncat(linfo.m_osinfo, uts.machine, sizeof(linfo.m_osinfo));
			strncpy(linfo.m_hostinfo, uts.nodename, sizeof(linfo.m_hostinfo));
		}
		
		ResourcePool::Get().GetDevicesNo(&linfo.m_gputhreads, &linfo.m_cputhreads);
		linfo.m_type = COMPUTE_TYPE_CLIENT;
	}  
	{
		struct sockaddr_in addr;
		socklen_t len2 = sizeof(addr);
		getsockname(sck, (sockaddr *)&addr, &len2);
		strncpy(linfo.m_ip, inet_ntoa(addr.sin_addr), sizeof(linfo.m_ip));
		linfo.m_port = ntohs(addr.sin_port);
		
		CLog::Log(LOG_LEVEL_NOMAL, "Client: %s %s %s:%d\n", linfo.m_osinfo, linfo.m_hostinfo,
					linfo.m_ip, linfo.m_port);
	}
	
	short status;
	char buf[1024];
	int n = Write(cmd, &linfo, sizeof(linfo));
	int m = Read(&cmd, &status, buf, sizeof(buf));
	CLog::Log(LOG_LEVEL_NOMAL, "Client: Read login %d %d\n", m, cmd);
			
	return 0;
}

bool Client::WillFetchItemFromServer()const
{
	if(connected != 2)
		return false;
		
	return fetch;
}

static int read_timeout(int fd, unsigned int wait_seconds)
{    
	int ret = 0;    
	if (wait_seconds > 0)    
	{             
		fd_set read_fdset;        
		struct timeval timeout;             
		
		FD_ZERO(&read_fdset);        
		FD_SET(fd, &read_fdset);             
		timeout.tv_sec = wait_seconds;        
		timeout.tv_usec = 0;             
		
		ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout); 
		
		if (ret == 0){            
			ret = -1;            
			errno = ETIMEDOUT;        
		} else if (ret == 1)            
			return 0;         
	}         
	return ret;
}


int Client::DownloadFile(const char* filename, const char* path)
{
	if(connected != 2)
		return ERR_CONNECTIONLOST;
	
	unsigned char cmd = CMD_DOWNLOAD_FILE;
	short status;
	char buf[4096];
	int m, n;
	
	Lock lk(&mutex);
	
	//第一步：请求下载文件，得到文件信息
	m = Write(CMD_DOWNLOAD_FILE, filename, strlen(filename)+1);
	if(m < 0)	return m;
	n = Read(&cmd, &status, buf, sizeof(buf));
	if(n < 0) return n;
	if(cmd != CMD_DOWNLOAD_FILE || status != 0)
		return ERR_DOWNLOADFILE;
	file_info info;
	memcpy(&info, buf, sizeof(info));
	CLog::Log(LOG_LEVEL_NOMAL, "download: %s [fd=%p len=%d]\n", filename, info.f, info.len);
	
	//第二步：传输文件
	char file[512];
	sprintf(file, "%s/%s", path, filename);
	FILE* fd = fopen(file, "w");
	file_info fi = {0};
	fi.f = info.f;
	fi.len = 4096; 
	int step = info.len / 40960 * 40960 / 10;
	if(step < 4096) step = 4096;
	unsigned int total = 0;
	bool failed = false;
	while(1)
	{
		fi.offset = total;
		if(fi.offset % step == 0 || fi.offset >= info.len)
			CLog::Log(LOG_LEVEL_NOMAL, " %d VS %d\n", fi.offset, info.len);
		if(fi.offset >= info.len)
			break;
			
		if(fi.offset == 0)
		{
			if((m = Write(CMD_START_DOWNLOAD, &fi, sizeof(fi))) < 0)
			{
				failed = true;
				CLog::Log(LOG_LEVEL_WARNING, "downloadfile: write err %d\n", m);
				break;
			}
		}
		
		if((n = Read(&cmd, &status, buf, sizeof(buf))) < 0 ||
			cmd != CMD_START_DOWNLOAD || status != 0)
		{
			failed = true;
			CLog::Log(LOG_LEVEL_WARNING, "downloadfile: read err %d %d %d\n", n, cmd, status);
			break;
		}
		
		if(fd) fwrite(buf, 1, n, fd);
		total += n;
	}
	
	//第三步:结束传输
	m = Write(CMD_END_DOWNLOAD, &info, sizeof(info));
	m = Read(&cmd, &status, buf, sizeof(buf));
	if(fd) fclose(fd);
	
	if(failed)
	{	
		unlink(file);
		return ERR_DOWNLOADFILE;
	}
	
	return 0;
}

int Client::Read(unsigned char *cmd, short* status, void* data, int size, int* seq)
{
	if(connected != 2)
		return ERR_CONNECTIONLOST;
	
	if(read_timeout(sck, 5) < 0)
		return ERR_TIMEOUT;
		
	control_header hdr;
	if(read(sck, &hdr, sizeof(hdr)) < 0) 
		return ERR_CONNECTIONLOST;
	
	if(memcmp(hdr.magic, pack_flag, 5) != 0)
		return ERR_INVALIDDATA;
		
	*cmd = hdr.cmd;
	*status = hdr.response;
	int totalN = hdr.compressLen;
	int origN = hdr.dataLen;
	if(seq)	*seq = hdr.seq;
	//printf("origN %d compressN %d seq %d\n", totalN, origN, hdr.seq);
	
	if(origN < -1 || size < origN)
		return ERR_INVALIDDATA;
	if(origN == 0 || origN == -1)
		return 0;
	
	unsigned char* buf = NULL;
	int m;
	if(totalN == -1) 
	{
		buf = (unsigned char*)data;
		m = origN;
	}
	else
	{
		buf = new unsigned char[totalN];
		m = totalN;
	}
	
	if(read_timeout(sck, 1) < 0)
		return ERR_TIMEOUT;
		
	int total = 0;
	int n;
	do{	
		if((n=read(sck, buf+total, m-total)) < 0)
		{
			delete []buf;
			if(errno == EAGAIN)
				return ERR_TIMEOUT;
			return ERR_CONNECTIONLOST;
		}
		total += n;
		if(total == m) break;
	}while(1);	
	
	if(totalN == -1) 
		return m; 
	
	unsigned long uncompressLen = size;
	int ret = uncompress((Bytef*)data, (uLongf*)&uncompressLen, buf, totalN);
	delete []buf;
	if(ret != 0 || uncompressLen != origN)
		return ERR_UNCOMPRESS;
	
	return uncompressLen;
}

int Client::Write(unsigned char cmd, const void* data, int size, int seq)
{
	if(connected != 2)
		return ERR_CONNECTIONLOST;
	
	struct control_header hdr = INITIALIZE_EMPTY_HEADER(cmd);
	hdr.seq = seq;
	if(!data || size == 0 || size == -1)
	{
		hdr.dataLen = size;
		if(write(sck, &hdr, sizeof(hdr)) < 0)
		{	
			connected = 0;
			close(sck);
			return ERR_CONNECTIONLOST;
		}
		return 0;
	}
	
	unsigned long destLen = compressBound(size);	
	unsigned char* dest = new unsigned char[destLen];
	int ret = compress(dest, &destLen, (const Bytef*)data, size);
	if(ret != 0)
	{
		delete []dest;
		return ERR_COMPRESS;
	}
	hdr.dataLen = size;
	hdr.compressLen = destLen;
	
	if(write(sck, &hdr, sizeof(hdr)) < 0 || write(sck, dest, destLen) < 0)
	{
		connected = 0;
		close(sck);
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return destLen;
}

int Client::ReportStatusToServer(crack_status* status)
{
	Lock lk(&mutex);
	
	unsigned char cmd = CMD_WORKITEM_STATUS;
	int m = Write(cmd, status, sizeof(*status));
	if(m == ERR_CONNECTIONLOST) 
		return m;
	
	short st;
	char buf[1024];
	Read(&cmd, &st, buf, sizeof(buf));
	return m;
}
	
int Client::ReportResultToServer(crack_result* result)
{
	Lock lk(&mutex);
	
	unsigned char cmd = CMD_WORKITEM_RESULT;
	int m = Write(cmd, result, sizeof(*result));
	if(m <= 0)
		return m;
	
	short status;
	char buf[1024];
	int n = Read(&cmd, &status, buf, sizeof(buf));
	if(n < 0)
		return n;
	return m;
}

int Client::GetWorkItemFromServer(crack_block* item)
{
	char buffer[1024*2] = {0};
	unsigned char cmd = CMD_GET_A_WORKITEM;
	short status;
	Lock lk(&mutex);
	
	int m = Write(cmd, NULL, 0);
	if(m < 0) 
		return m;
	
	int n = Read(&cmd, &status, buffer, sizeof(buffer));
	CLog::Log(LOG_LEVEL_NOMAL, "Client: Read workitem %d %d %d\n", cmd, n, sizeof(*item));
		
	if(cmd == CMD_GET_A_WORKITEM && status == 0)
	{
		memcpy(item, buffer, sizeof(*item));
		return n;
	}
	return n;
}

bool Client::Connected()const
{
	return connected == 2;
}

