#include "StdAfx.h"
#include "SockServer.h"
#include "server_function.h"
#include "CLog.h"


CSockServer::CSockServer(void)
{
}

CSockServer::~CSockServer(void)
{
}

UINT CSockServer::Initialize(UINT port,UINT maxConnNum)
{
	
	m_nPort = port;
	m_nMaxConnNum = maxConnNum;
	m_nMaxThread = maxConnNum;
	m_nCurrentThread = 0;
	return 0;
}

INT CSockServer::StartServer(void)
{
	//建立SOCKET 监听 ,接收客户端的连接
	UINT nRet = 0;
	WSADATA ws;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr,clientAddr;
	int addrLen = 0;
	HANDLE hThread = NULL;

	CLog::Log(LOG_LEVEL_WARNING,"Start Server ....\n");
	
	nRet = WSAStartup(MAKEWORD(2,2) , &ws);
	if (0 != nRet ){

		CLog::Log(LOG_LEVEL_WARNING,"Init Server Socket Lib Error.\n");
		//printf("init windows sock error!\n");
		return -1;
	}
	
	CLog::Log(LOG_LEVEL_WARNING,"Init Server Socket Lib OK\n");

	m_ListenSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_ListenSock == INVALID_SOCKET){
		
		CLog::Log(LOG_LEVEL_WARNING,"Create Server Socket Error.\n");
		//printf("create socket error!\n");
		return -2;

	}


	serverAddr.sin_port = htons(m_nPort);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	nRet = bind(m_ListenSock,(SOCKADDR *)&serverAddr,sizeof(SOCKADDR));
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Bind Server Socket Error.\n");
		return -3;

	}

	nRet = listen(m_ListenSock,10);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Server Listen Error.\n");
		return -4;

	}

	while(true){

		
		addrLen = sizeof(clientAddr);
		clientSocket = accept(m_ListenSock,(SOCKADDR *)&clientAddr,&addrLen);
		if (clientSocket == INVALID_SOCKET){

			CLog::Log(LOG_LEVEL_WARNING,"Accept Client Connect Error.\n");
			return -6;
		}

		m_nCurrentThread +=1 ;

		CLog::Log(LOG_LEVEL_WARNING,"Client Conn count is : %d\n",m_nCurrentThread);

	/*	if (m_nCurrentThread > m_nMaxThread){
				
			closesocket(clientSocket);  //关闭客户端套接字
			
			CLog::Log(LOG_LEVEL_WARNING,"Client Conn Reach Limit %d.\n",m_nMaxThread);
			//	printf("Client Connection count is %d , reach the max limit.\n",m_nCurrentThread);
			Sleep(10000);
			continue;

		}
	*/
		
		CLog::Log(LOG_LEVEL_WARNING,"Client Addr is : %s\n",inet_ntoa(clientAddr.sin_addr));

		//printf("Client Connect : %s\n",inet_ntoa(clientAddr.sin_addr));
		
		SOCKET* s = new SOCKET(clientSocket);
		hThread = CreateThread(NULL,0,reinterpret_cast<LPTHREAD_START_ROUTINE>(&ProcessClientData1),(LPVOID)s,0,NULL);
		if (hThread == 0){

			CLog::Log(LOG_LEVEL_WARNING,"Create Thread Process Client Connection Error.\n");
			break;

		}

		CloseHandle(hThread);

	}

	return 0;
}

int CSockServer::ShutDown(void)
{
	closesocket(m_ListenSock);
	WSACleanup();
	return 0;
}


