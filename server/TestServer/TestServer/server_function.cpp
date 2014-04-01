

#include "stdafx.h"

#include "macros.h"
#include "algorithm_types.h"
#include "err.h"
#include "Common.h"
#include "server_function.h"
#include "CLog.h"
#include "PacketProcess.h"
#include "zlib.h"
#include "ServerResp.h"
#include "err.h"
#include "ServerResp.h"

#pragma comment(lib,"zlib.lib")


VOID ProcessClientData1(LPVOID lpParameter){

	SOCKET cliSocket = *(SOCKET *)lpParameter;
	
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
	strcpy(ip, inet_ntoa(addr.sin_addr));
	int port = ntohs(addr.sin_port);

	while(1)
	{
		int m = Read(cliSocket, &cmd, &status, recvBuf, sizeof(recvBuf));
		CLog::Log(LOG_LEVEL_WARNING, "%s:%d recv cmd %d status %d body %d\n",ip, port, cmd, status, m);

		if(m == ERR_CONNECTIONLOST) break;//�Ƴ���

		doRecvData(lpParameter, recvBuf, m, cmd);
	}

	CLog::Log(LOG_LEVEL_WARNING, "Quit %s %d\n", __FUNCTION__, cliSocket);

}
