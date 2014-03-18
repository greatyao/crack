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

static unsigned char pack_flag[5] = {'G', '&', 'C', 'P', 'U'};
static const int HDR_SIZE = 13;

Client& Client::Get()
{
	static Client client;
	return client;
}

Client::Client()
{
	sck = 0;
	connected = false;
}

Client::~Client()
{
}

int Client::Connect(const char* ip, unsigned short port)
{
	if(connected != 0)
		return 0;
	
	connected = 1;
	strncpy(this->ip, ip, 16);
	this->port = port;
	
	sck = socket(AF_INET, SOCK_STREAM, 0);
	if(sck < 0)
	{
		CLog::Log(LOG_LEVEL_WARNING, "client: failed to create socket");
		connected = 0;
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
		CLog::Log(LOG_LEVEL_WARNING, "client: failto connect to %s:%d\n", ip, port);
		connected = 0;
		return ERR_INTERNALCLIENT;
	}
	
	connected = 2;
	return 0;
}

int Client::Read(void* data, int size)
{	
	unsigned char hdr[HDR_SIZE];
	if(read(sck, hdr, sizeof(hdr)) < 0)
		return ERR_CONNECTIONLOST;
	
	if(memcmp(hdr, pack_flag, 5) != 0)
		return ERR_INVALIDDATA;
		
	int totalN = *((int*)(hdr+5));
	int origN = *((int*)(hdr+9));
	
	totalN -= HDR_SIZE;
	if(totalN <= 0 || origN <= 0)
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
	unsigned char* dest = new unsigned char[destLen];
	int ret = compress(dest, &destLen, (const Bytef*)data, size);
	if(ret != 0)
	{
		delete []dest;
		return ERR_COMPRESS;
	}
	
	//数据包格式为：flag|packetlen|unziplen|zipdata
	unsigned bufLen = HDR_SIZE + destLen;
	if(write(sck, pack_flag, sizeof(pack_flag)) < 0 ||
		write(sck, &bufLen, sizeof(bufLen)) < 0 ||
		write(sck, &size, sizeof(size)) < 0 ||
		write(sck, dest, destLen) < 0){
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return bufLen;
}

