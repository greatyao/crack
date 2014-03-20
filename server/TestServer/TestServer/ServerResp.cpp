


#include "stdafx.h"

#include "ServerResp.h"
#include "CLog.h"


//发送数据到对端接口
int SendDataToPeer(void *pclient, unsigned char * pdata, unsigned int len){


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





