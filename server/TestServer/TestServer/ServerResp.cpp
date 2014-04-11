#include "stdafx.h"
#include "macros.h"
#include "err.h"
#include "ServerResp.h"
#include "CLog.h"
#include "zlib.h"
#include "algorithm_types.h"
#include "Common.h"
#include "PacketProcess.h"

static unsigned char pack_flag[5] = {'G', '&', 'C', 'P', 'U'};

int Read(int sck, unsigned char *cmd, short* status, void* data, int size)
{
	control_header hdr;
	if(recv(sck, (char*)&hdr, sizeof(hdr), 0) <= 0) 
		return ERR_CONNECTIONLOST;
	
	if(memcmp(hdr.magic, pack_flag, 5) != 0)
		return ERR_INVALIDDATA;
		
	*cmd = hdr.cmd;
	*status = hdr.response;
	int totalN = hdr.compressLen;
	int origN = hdr.dataLen;
	
	if(origN < 0 || size < origN)
		return ERR_INVALIDDATA;
	if(origN == 0)
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
		if((n=recv(sck, (char *)buf+total, m-total, 0)) <= 0)
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

static int mysend(int sck, void* buf, int size, int flag)
{
	int total = 0;
	int n;
	do{	
		if((n=send(sck, (char *)buf+total, size-total, flag)) < 0)
			return -1;
		total += n;
		if(total == size) break;
	}while(1);	

	return size;
}

int Write(int sck, unsigned char cmd, short status, const void* data, int size,bool bCompress)
{
	struct control_header hdr = INITIALIZE_EMPTY_HEADER(cmd);
	hdr.response = status;
	if(!data || size == 0)
	{
		if(mysend(sck, (char *)&hdr, sizeof(hdr), 0) < 0)
			return ERR_CONNECTIONLOST;
		return 0;
	}
	
	unsigned long destLen = 1.1*size+8;	
	unsigned char* dest = new unsigned char[destLen];
	unsigned char *pBuf = NULL;
	int ret = 0;

	if (bCompress){
		ret = compress(dest, &destLen, (const Bytef*)data, size);
		if(ret != 0)
		{
			delete []dest;
			return ERR_COMPRESS;
		}

		pBuf = dest;
		
		hdr.compressLen = destLen;
	}else {

		
		hdr.compressLen = -1;
		destLen = size;
		pBuf =(unsigned char *)data;
		
	}
	
	hdr.dataLen = size;
	
	if(mysend(sck, (char *)&hdr, sizeof(hdr), 0) < 0 || mysend(sck, (char *)pBuf, destLen, 0) < 0)
	{
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return destLen;
}


void DispatchThread(void* p){
	SOCKET cliSocket = *(SOCKET *)p;
	
	INT nRet = 0;
	UINT len = 0;

	BYTE recvBuf[MAX_BUF_LEN*4];
	INT cmdheader = sizeof(control_header);
	unsigned char cmd;
	short status;
	
	struct sockaddr_in addr;
	int len2 = sizeof(addr);
	getpeername(cliSocket, (sockaddr *)&addr, &len2);
	char ip[16];
	memset(ip,0,16);
	strcpy(ip, inet_ntoa(addr.sin_addr));
	int port = ntohs(addr.sin_port);

	while(1)
	{
		int m = Read(cliSocket, &cmd, &status, recvBuf, sizeof(recvBuf));
	
		if(m == ERR_CONNECTIONLOST) {
			cmd = CMD_CLIENT_QUIT;
			memset(recvBuf,0,36000);
			memcpy(recvBuf,ip,strlen(ip));
			m = port;
			doRecvData(p,recvBuf,m,cmd);
			break;//ÍÆ³öÁË
		}else if(m == ERR_INVALIDDATA || m == ERR_UNCOMPRESS)
			continue;
		
		//CLog::Log(LOG_LEVEL_WARNING, "%s:%d recv cmd %d status %d body %d\n",ip, port, cmd, status, m);

		doRecvData(p, recvBuf, m, cmd);
	}

	closesocket(cliSocket);
	delete p;
	CLog::Log(LOG_LEVEL_WARNING, "Client [%s:%d] Quit!\n",ip,port);
}
