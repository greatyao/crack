#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <zlib.h>

#include "Client.h"
#include "err.h"
#include "CLog.h"
#include "macros.h"
#include "algorithm_types.h"

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
	tid = 0;
	pthread_mutex_init(&mutex, NULL);
}

Client::~Client()
{
}

void* Client::MonitorThread(void* p)
{
	Client* client = (Client*)p;
	int ret;
	char buf[1024];
		
	sleep(5);

conn:	
	while(client->connected != 2)
	{
		CLog::Log(LOG_LEVEL_NOMAL, "client: reconnect to server\n");
		ret = client->Connect(client->ip, client->port);
			
		sleep(10);
	}
	CLog::Log(LOG_LEVEL_NOMAL, "------------------------------\n");
		
	while(1)
	{
		sleep(10);
		
		Lock lk(&client->mutex);
		unsigned char cmd = TOKEN_HEARTBEAT;
		int n = client->Write(&cmd, sizeof(cmd));
		if(n == ERR_CONNECTIONLOST) 
		{
			client->connected = 0;
			goto conn;
		}
		
		int m = client->Read(buf, sizeof(buf));
		printf("read %d %d\n", m, buf[0]);
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
		CLog::Log(LOG_LEVEL_WARNING, "client: failed to create socket");
		connected = 0;
		close(sck);
		return ERR_INTERNALCLIENT;
	}
	
	struct sockaddr_in s_add;
	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = inet_addr(ip);
	s_add.sin_port = htons(port);
	CLog::Log(LOG_LEVEL_NOMAL, "client: connecting to %s:%d\n", ip, port);

	if(connect(sck,(struct sockaddr *)&s_add, sizeof(struct sockaddr)))
	{
		CLog::Log(LOG_LEVEL_WARNING, "client: failed to connect to %s:%d\n", ip, port);
		connected = 0;
		close(sck);
		return ERR_INTERNALCLIENT;
	}
	
	connected = 2;
	
	//struct timeval timeout = {3, 0};
	//setsockopt(sck, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	
	unsigned char cmd = TOKEN_LOGIN;
	int n = Write(&cmd, sizeof(cmd));
	int m = Read(&cmd, sizeof(cmd));
	
	return 0;
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


int Client::Read(void* data, int size)
{	
	unsigned char hdr[HDR_SIZE];
	
	if(read_timeout(sck, 5) < 0)
		return ERR_TIMEOUT;
		
	if(read(sck, hdr, sizeof(hdr)) < 0)
		return ERR_CONNECTIONLOST;
	
	if(memcmp(hdr, pack_flag, 5) != 0)
		return ERR_INVALIDDATA;
		
	int totalN = *((int*)(hdr+5));
	int origN = *((int*)(hdr+9));
	
	totalN -= HDR_SIZE;
	if(totalN <= 0 || origN <= 0 || size < origN)
		return ERR_INVALIDDATA;
		
	unsigned char* buf = new unsigned char[totalN];
	if(read(sck, buf, totalN) < 0)
		return ERR_CONNECTIONLOST;
		
	int ret = uncompress((Bytef*)data, (uLongf*)&size, buf, totalN);
	delete []buf;
	if(ret != 0 || size != origN)
		return ERR_UNCOMPRESS;
		
	return size;
}	

int Client::Write(const void* data, int size)
{
	unsigned long destLen = 1.001*size + 12;
	unsigned char* dest = new unsigned char[HDR_SIZE+destLen];
	int ret = compress(dest+HDR_SIZE, &destLen, (const Bytef*)data, size);
	if(ret != 0)
	{
		delete []dest;
		return ERR_COMPRESS;
	}
	
	//数据包格式为：flag|packetlen|unziplen|zipdata
	unsigned bufLen = HDR_SIZE + destLen;
	memcpy(dest, pack_flag, sizeof(pack_flag));
	memcpy(dest+5, &bufLen, sizeof(bufLen));
	memcpy(dest+9, &size, sizeof(size));
	if(write(sck, dest, bufLen) < 0){
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return bufLen;
}

int Client::GetWorkItemFromServer(void* data, int size)
{
	crack_block* item = (crack_block*)data;
	char buffer[1024*2] = {0};
	unsigned char cmd = CMD_GET_A_WORKITEM;
	Lock lk(&mutex);
	
	int m = Write(&cmd, sizeof(cmd));
	if(m < 0) 
	{
		if(m == ERR_CONNECTIONLOST) connected = 0;
		return m;
	}
	
	int n = Read(buffer, sizeof(buffer));
	
	static crack_block all_items[] = {
		{algo_sha1,		charset_num,	bruteforce,  0, "D", "8cb2237d0679ca88db6464eac60da96345513964", 1, 7, 0, 0},
		{algo_lm,		charset_num,	bruteforce,  0, "01", "AEBD4DE384C7EC43AAD3B435B51404EE", 1, 6, 0, 0},
		{algo_oscommerce,charset_num,	bruteforce,  0, "02", "d6b0ab7f1c8ab8f514db9a6d85de160a:abc", 1, 6, 0, 0},
		{algo_desunix,	charset_num,	bruteforce,  0,	"03", "27EP4PuToKUSI", 1, 6, 0, 0},
		{algo_desunix,	charset_num,	bruteforce,  0,	"04", "27EP4PuToKUSi", 1, 6, 0, 0},
		{algo_nsldap,	charset_num,	bruteforce,  0, "05", "{SHA}jLIjfQZ5yojbZGTqxg2pY0VROWQ=", 1, 7, 0, 0},
		{algo_nsldaps,	charset_num,	bruteforce,  0,	"06", "{SSHA}P2oLaAFiExs+MexEjZ5R+KYbtrBhSHhzWnM4VA==", 1, 6, 0, 0},
		{algo_md4,		charset_num,	bruteforce,  0, "07", "23580e2a459f7ea40f9efa148b63cafb", 1, 6, 0, 0},
		{algo_md5,		charset_num,	bruteforce,  0, "08", "827ccb0eea8a706c4c34a16891f84e7b", 1, 6, 0, 0},
		{algo_md5md5,	charset_num,	bruteforce,  0, "09", "1f32aa4c9a1d2ea010adcf2348166a04", 1, 6, 0, 0},
		//{algo_mssql_2000,charset_num,	bruteforce,  0, "9", "0x010077393477CB2764D565692A0DE0D5308C3E19FEE51223EC1ACB2764D565692A0DE0D5308C3E19FEE51223EC1A", 1, 6, 0},
		{algo_mssql_2005,charset_num,	bruteforce,  0, "A", "0x01004D53456421450CD84AB5AF29A49A90BDBC1AFB0EFBDAF259", 1, 6, 0, 0},
		{algo_mysql5,	charset_num,	bruteforce,  0, "B", "*00a51f3f48415c7d4e8908980d443c29c69b60c9", 1, 7, 0, 0},
		{algo_pixmd5,	charset_num,	bruteforce,  0, "C", "u0-pixmd5:UwiM/pkFcM.xYc8s", 1, 7, 0, 0},
		//{algo_sha512,	charset_num,	bruteforce,  0, "E", "3627909a29c31381a071ec27f7c9ca97726182aed29a7ddd2e54353322cfb30abb9e3a6df2ac2c20fe23436311d678564d0c8d305930575f60e2d3d048184d79", 1, 7, 0}
	};
	static int mm = 0;
	memcpy(item, &all_items[mm], sizeof(*item));
	mm = (++mm)  % (sizeof(all_items)/sizeof(all_items[0]));
					
	return n;
}