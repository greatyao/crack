#include "StdAfx.h"
#include "SocketClient.h"
#include "err.h"
#include "macros.h"
#include "zlib.h"
#include "CLog.h"

#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"ws2_32.lib")

static unsigned char pack_flag[5] = {'G', '&', 'C', 'P', 'U'};


CSocketClient::CSocketClient(void)
{
}

CSocketClient::~CSocketClient(void)
{
}



int CSocketClient::Init(char *ip,int port){

	WSADATA ws;
	int Ret = 0;
	struct sockaddr_in ServerAddr;

	if (WSAStartup(MAKEWORD(2,2),&ws)!= 0){

	//	CLog::Log(LOG_LEVEL_WARNING,"WSAStartup Error\n");
		return -1;
	}

	m_clientsocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_clientsocket == INVALID_SOCKET){

	//	CLog::Log(LOG_LEVEL_WARNING,"Create Client Socket Error\n");
		return -2;

	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(ip);
	ServerAddr.sin_port = htons(port);
	memset(ServerAddr.sin_zero,0x00,8);

	Ret = connect(m_clientsocket,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr));
	if (Ret == SOCKET_ERROR){

	//	CLog::Log(LOG_LEVEL_WARNING,"Connect Server Error\n");
		return -3;

	}else{

		//CLog::Log(LOG_LEVEL_WARNING,"Connect Server %s:%d OK\n",ip,port);
	}
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
		} else if (ret == 1)            
			return 0;         
	}         
	return ret;
}

int CSocketClient::Read(unsigned char *cmd, short* status, void* data, int size, unsigned int* seq)
{
	if(read_timeout(m_clientsocket, 5) < 0)
		return ERR_TIMEOUT;
	
	control_header hdr;
	if(recv(m_clientsocket, (char*)&hdr, sizeof(hdr), 0) <= 0) 
		return ERR_CONNECTIONLOST;
	
	if(memcmp(hdr.magic, pack_flag, 5) != 0)
		return ERR_INVALIDDATA;
		
	*cmd = hdr.cmd;
	*status = hdr.response;
	int totalN = hdr.compressLen;
	int origN = hdr.dataLen;
	if(seq)	*seq = hdr.seq;
	
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
	
	int total = 0;
	int n;
	do{	
		if((n=recv(m_clientsocket, (char *)buf+total, m-total, 0)) <= 0)
		{
			delete []buf;
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

static int mysend(SOCKET s, void* buf, int size, int flag)
{
	int total = 0;
	int n;
	do{	
		if((n=send(s, (char *)buf+total, size-total, flag)) < 0)
		{
			CLog::Log(LOG_LEVEL_WARNING, "failed to send %d\n", GetLastError());
			return -1;
		}
		total += n;
		if(total == size) break;
	}while(1);	

	return size;
}

int CSocketClient::Write(unsigned char cmd, short status, void* data, int size, unsigned int seq)
{
	struct control_header hdr = INITIALIZE_EMPTY_HEADER(cmd);
	hdr.response = status;
	hdr.seq = seq;
	if(!data || size == 0 || size == -1)
	{
		hdr.dataLen = size;
		if(mysend(m_clientsocket, (char *)&hdr, sizeof(hdr), 0) < 0)
			return ERR_CONNECTIONLOST;
		return 0;
	}
	
	unsigned long destLen = (unsigned long)1.1*size+8;	
	unsigned char* dest = new unsigned char[destLen];
	int ret = compress(dest, &destLen, (const Bytef*)data, size);
	if(ret != 0)
	{
		delete []dest;
		return ERR_COMPRESS;
	}
	hdr.dataLen = size;
	hdr.compressLen = destLen;
	
	if(mysend(m_clientsocket, (char *)&hdr, sizeof(hdr), 0) < 0 || mysend(m_clientsocket, (char *)dest, destLen, 0) < 0)
	{
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return destLen;
}

int CSocketClient::WriteNoCompress(unsigned char cmd, short status, void* data, int size, unsigned int seq)
{
	struct control_header hdr = INITIALIZE_EMPTY_HEADER(cmd);
	hdr.response = status;
	if(!data || size == 0|| size == -1)
	{
		hdr.dataLen = size;
		if(mysend(m_clientsocket, (char *)&hdr, sizeof(hdr), 0) < 0)
			return ERR_CONNECTIONLOST;
		return 0;
	}
	
	
	hdr.dataLen = size;
	hdr.compressLen = -1;
	
	if(mysend(m_clientsocket, (char *)&hdr, sizeof(hdr), 0) < 0 || mysend(m_clientsocket, (char *)data, size, 0) < 0)
	{
		return ERR_CONNECTIONLOST;
	}
	
	return size;
}



int CSocketClient::Finish(){
	
	closesocket(m_clientsocket);
	return 0;
}