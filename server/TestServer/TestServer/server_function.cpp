

#include "stdafx.h"

#include "Common.h"
#include "server_function.h"
#include "CLog.h"
#include "PacketProcess.h"
#include "zlib.h"
#pragma comment(lib,"zlib.lib")


VOID ProcessClientData(LPVOID lpParameter){

	SOCKET cliSocket = *(SOCKET *)lpParameter;
	
	INT nRet = 0;
	UINT len = 0;

	BYTE recvBuf[MAX_BUF_LEN];

	while(true){

		memset(recvBuf,0,MAX_BUF_LEN);
		nRet = recv(cliSocket,(char *)recvBuf,MAX_BUF_LEN,0);
		if(nRet == 0 || nRet == SOCKET_ERROR){

			CLog::Log(LOG_LEVEL_WARNING,"Client %d Quit.\n",cliSocket);
//			printf("Client Quit!\n");
	
	//		g_Server->m_nCurrentThread -=1;
	//		连接数减少
			
			break;
		}else{
			
			len = nRet;
			CLog::Log(LOG_LEVEL_WARNING,"Recv Data Len :%d.\n",len);
		//	printf("Recv Len : %d \n",nRet);


		}

		//gen resposne data, then send to client
		nRet = SendServerData((LPVOID)&cliSocket,recvBuf,len);
		if (nRet != 0){

			CLog::Log(LOG_LEVEL_WARNING,"Server Process Data Error\n");
			closesocket(cliSocket);
			break;
		}

	}

	return ;

}


INT SendServerData(LPVOID pClient, LPBYTE pData, UINT len)
{

	/*

	//for test the socket 


	SOCKET cliSocket = *(SOCKET *)pClient;
	
	BYTE sendBuf[MAX_BUF_LEN];

	memset(sendBuf,0,MAX_BUF_LEN);

	memcpy(sendBuf,pData,len);
	
	memcpy(sendBuf+len,":hello",6);


	send(cliSocket,(char *)sendBuf,len+6,0);
	
	*/
	
	UINT nRet = 0;
	NET_BUFF_HEADER *pBufHeader = NULL;
	UINT nCompressLen = 0;
	UINT nUnCompressLen = 0;
	UINT nTotalLen = 0;
	LPBYTE pUNCompressBuf = NULL;
	ULONG lLen = 0;
	
	if (len < HEADER_SIZE){

		CLog::Log(LOG_LEVEL_WARNING,"Buffer Len at least 13.\n");
		return -1;
	}
	
	pBufHeader = new NET_BUFF_HEADER;
	if (NULL == pBufHeader){

		CLog::Log(LOG_LEVEL_WARNING,"Create Buffer Error.\n");
		return -1;
	}
	CopyMemory(pBufHeader,pData,sizeof(NET_BUFF_HEADER));
	if (memcmp(pBufHeader->tag,BUFF_TAG,5) != 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"BUFF Tag is Error\n");
		delete pBufHeader;
		return -1;

	}
	
	delete pBufHeader;

	nTotalLen = pBufHeader->totallen;
	nUnCompressLen = pBufHeader->uncompresslen;


	//uncompress buffer 
	nCompressLen = nTotalLen - HEADER_SIZE;
	
	pUNCompressBuf = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nUnCompressLen);
	if (!pUNCompressBuf){

		CLog::Log(LOG_LEVEL_WARNING,"Create UNCompress Buff Error\n");
		return -2;
	}
	
	nRet = uncompress(pUNCompressBuf,&lLen,&pData[HEADER_SIZE],nCompressLen);
	if (nRet != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		HeapFree( GetProcessHeap(),HEAP_NO_SERIALIZE,pUNCompressBuf);
		return -3;

	}

	nUnCompressLen = lLen;
	//处理请求报文，生成应答报文
	nRet = 0;
	nRet = process_recv_data(pClient,pUNCompressBuf,nUnCompressLen);
	if (nRet != 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Process Recv Data Error\n");
		HeapFree( GetProcessHeap(),HEAP_NO_SERIALIZE,pUNCompressBuf);
		return -4;

	}
	
	HeapFree( GetProcessHeap(),HEAP_NO_SERIALIZE,pUNCompressBuf);
	CLog::Log(LOG_LEVEL_WARNING,"Process Recv Data OK.\n");

	return 0;
}


//处理请求，生成应答并发送

INT process_recv_data(LPVOID pClient, LPBYTE pData, UINT len){

	BYTE bCmd = pData[0];   //获得请求命令字
	
	INT nRet = 0;
	

	nRet = doRecvData(pClient,pData,len,bCmd);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Recv Data Done Error\n");
		return -1;
	}


	CLog::Log(LOG_LEVEL_WARNING,"Recv Data Done OK\n");

	return 0;
}




