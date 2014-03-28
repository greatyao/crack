


#include "stdafx.h"

#include "ServerResp.h"
#include "CLog.h"


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



