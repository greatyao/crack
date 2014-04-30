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
#include <deque>

#include "Client.h"
#include "CrackManager.h"
#include "resourceslotpool.h"
#include "err.h"
#include "CLog.h"
#include "macros.h"
#include "algorithm_types.h"

static std::deque<crack_block> blocks;

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

const struct timespec tensecs = {10, 0};  

class Wait{
public:
	Wait(pthread_mutex_t* mutex, pthread_cond_t* cond)
	{
		pthread_cond_timedwait(cond, mutex, &tensecs);
	}
};

class Signal{
public:
	Signal(pthread_cond_t* cond)
	{
		pthread_cond_signal(cond);
	}
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
	tid2 = 0;
	fetch = true;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
}

Client::~Client()
{
	Destory();
}

void Client::Destory()
{
	if(tid || tid2)
	{
		stop = true;
		if(tid){
			pthread_join(tid, NULL);
			tid = 0;
			CLog::Log(LOG_LEVEL_NOMAL, "Client: Succeed to exit monitor thread\n");
		}
		if(tid2){
			pthread_join(tid2, NULL);
			tid2 = 0;
			CLog::Log(LOG_LEVEL_NOMAL, "Client: Succeed to exit dispatch thread\n");
		}
	}
}

void* Client::DispatchThread(void* p)
{
	Client* client = (Client*)p;
	char buf[1024*4];
	time_t t0 = time(NULL);
	while(1)
	{
		if(client->stop) return NULL;
		
		if(client->connected != 2)
		{
			t0 = time(NULL);
			sleep(1);
		}
		
		unsigned char cmd;
		short status;
		Lock lk(&client->mutex);
		int m = client->Read(&cmd, &status, buf, sizeof(buf));
		
		if(m == ERR_CONNECTIONLOST) 
		{
			client->connected = 0;
			Signal(&client->cond); 
			continue;
		}
		else if(m == ERR_TIMEOUT)
		{
			Signal(&client->cond); 
			continue;
		}
		else if(m >= 0)
		{
			if(cmd == CMD_LOGIN)
			{
				CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: Read login %d\n", m);
			}
			else if(cmd == CMD_COMP_HEARTBEAT)
			{
				keeplive_compclient* ka = (keeplive_compclient*)buf;
				CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: Special Hearbreak [%d,%d]\n", ka->tasks, ka->blocks);
				client->fetch = (ka->tasks > 0);
				
				for(int i = 0; i < ka->blocks; i++)
				{
					CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: Stop workitem [guid=%s]\n", ka->guids[i]);
					CrackManager::Get().StopCrack(ka->guids[i]);
				}
			}
			else if(cmd == CMD_REPLAY_HEARTBEAT)
			{
				CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: Normal Hearbreak %d\n", m);
				client->fetch = true;
			}
			else if(cmd == CMD_WORKITEM_STATUS)
			{
				//CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: status %d\n", m);
			}
			else if(cmd == CMD_WORKITEM_RESULT)
			{
				CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: result %d\n", m);
				Signal(&client->cond); 
			}
			else if(cmd == CMD_GET_A_WORKITEM)
			{
				CLog::Log(LOG_LEVEL_NOMAL, "Dispatch: get workitem %d\n", m);
				if(status == 0 && m == sizeof(crack_block))
				{
					crack_block item;
					memcpy(&item, buf, sizeof(item));
					blocks.push_back(item);
				}
	
				Signal(&client->cond); 
			}
			else if(cmd == CMD_DOWNLOAD_FILE)
			{
			}
			else if(cmd == CMD_START_DOWNLOAD)
			{
			}
			else if(cmd == CMD_END_DOWNLOAD)
			{
			}
		}
		
		time_t t1 = time(NULL);
		if(t1 - t0 >= 600)
		{	
			t0 = t1;
			client->fetch = true;
		}
	}
	return NULL;
}

void* Client::MonitorThread(void* p)
{
	Client* client = (Client*)p;
	int ret;
		
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
	
	if(tid2 == 0)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&tid2, &attr, DispatchThread, (void *)this);
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
	CLog::Log(LOG_LEVEL_SUCCEED, "Client: connected to %s:%d\n", ip, port);
	
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
	
	Write(cmd, &linfo, sizeof(linfo));
			
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
	return 1;
}
	
int Client::ReportResultToServer(crack_result* result)
{
	Lock lk(&mutex);
	
	unsigned char cmd = CMD_WORKITEM_RESULT;
	int m = Write(cmd, result, sizeof(*result));
	if(m <= 0)
		return m;
	
	Wait(&mutex, &cond);  
	
	return 1;
}

int Client::GetWorkItemFromServer(crack_block* item)
{
	unsigned char cmd = CMD_GET_A_WORKITEM;
	int m = 0;
	Lock lk(&mutex);
	if(blocks.size() != 0)
		goto got;
	
	m = Write(cmd, NULL, 0);
	if(m < 0) 
		return m;
	
	Wait(&mutex, &cond); 
	
got:
	if(blocks.size() != 0)
	{
		int ssize = sizeof(crack_block);
		memcpy(item, &blocks[0], ssize);
		blocks.pop_front();
		return ssize;
	}
	return 0;
}

bool Client::Connected()const
{
	return connected == 2;
}

