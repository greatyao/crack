


#include "stdafx.h"
#include "macros.h"
#include "err.h"
#include "ServerResp.h"
#include "CLog.h"
#include "zlib.h"

static unsigned char pack_flag[5] = {'G', '&', 'C', 'P', 'U'};

int Read(int sck, unsigned char *cmd, short* status, void* data, int size)
{
	control_header hdr;
	if(recv(sck, (char*)&hdr, sizeof(hdr), 0) < 0) 
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
		if((n=recv(sck, (char *)buf+total, m-total, 0)) < 0)
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


int Write(int sck, unsigned char cmd, const void* data, int size)
{
	struct control_header hdr = INITIALIZE_EMPTY_HEADER(cmd);
	if(!data || size == 0)
	{
		if(send(sck, (char *)&hdr, sizeof(hdr), 0) < 0)
			return ERR_CONNECTIONLOST;
		return 0;
	}
	
	unsigned long destLen = 1.1*size+8;	
	unsigned char* dest = new unsigned char[destLen];
	int ret = compress(dest, &destLen, (const Bytef*)data, size);
	if(ret != 0)
	{
		delete []dest;
		return ERR_COMPRESS;
	}
	hdr.dataLen = size;
	hdr.compressLen = destLen;
	
	if(send(sck, (char *)&hdr, sizeof(hdr), 0) < 0 || send(sck, (char *)dest, destLen, 0) < 0)
	{
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return destLen;
}


//发送数据到对端接口
int SendDataToPeer1(void *pclient, unsigned char * pdata, unsigned int len){


	int nRet = 0;
	SOCKET peerSocket = *(SOCKET *)pclient;

	nRet = send(peerSocket,(char *)pdata,len,0);
	if (nRet == SOCKET_ERROR){

		CLog::Log(LOG_LEVEL_WARNING,"Send Data Error.\n");
		nRet = -1;
	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Send Data OK.\n");
	}
	return nRet;
}



int SendDataToPeer(void *pclient, unsigned char * pdata, unsigned int len){

	int nRet = 0;
	int sendLen = 0;
	SOCKET peerSocket = *(SOCKET *)pclient;


	while (sendLen < len ){
		nRet = send(peerSocket,(char *)pdata+sendLen,len-sendLen,0);
		if (nRet == SOCKET_ERROR){

			CLog::Log(LOG_LEVEL_WARNING,"Send Data Error.\n");
			nRet = -1;
			break;
		}else{

			if (nRet > 0){
				
				sendLen += nRet;
				
			}
			CLog::Log(LOG_LEVEL_WARNING,"Send Data %d OK.\n",sendLen);
		}
		

	}
	return nRet;
}


//接收数据
int RecvDataFromPeer(void *pclient, unsigned char * pdata, unsigned int len){


	int nRet = 0;
	int recvLen = 0;
	SOCKET peerSocket = *(SOCKET *)pclient;


	while (recvLen < len ){

		nRet = recv(peerSocket,(char *)pdata+recvLen,len-recvLen,0);
		if(nRet == 0 || nRet == SOCKET_ERROR){

			CLog::Log(LOG_LEVEL_WARNING,"Client %d Quit.\n",peerSocket);
			return -1;
			
		}else{

			if (nRet > 0){

				recvLen +=nRet;
			}
			
			CLog::Log(LOG_LEVEL_WARNING,"Recv Data Len :%d.\n",recvLen);

		}

	}
	return recvLen;
}



