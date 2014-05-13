#include "SockServer.h"
#include "ServerResp.h"
#include "CLog.h"
#include "macros.h"
#include "CrackBroker.h"
#include "ClientInfo.h"
#include <mstcpip.h>
#include "conn.h"

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
		return -1;
	}
	
	CLog::Log(LOG_LEVEL_WARNING,"Init Server Socket Lib OK\n");

	m_ListenSock = (SOCKET)conn_socket(CONN_TCP);
	//m_ListenSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_ListenSock == NULL){
		
		CLog::Log(LOG_LEVEL_WARNING,"Create Server Socket Error.\n");
		return -2;
	}

	serverAddr.sin_port = htons(m_nPort);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	nRet = conn_bind((conn)m_ListenSock,(SOCKADDR *)&serverAddr,sizeof(SOCKADDR));
	//nRet = bind(m_ListenSock,(SOCKADDR *)&serverAddr,sizeof(SOCKADDR));
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Bind Server Socket Error.\n");
		return -3;
	}

	nRet = conn_listen((conn)m_ListenSock, 5);
	//nRet = listen(m_ListenSock, 5);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Server Listen Error.\n");
		return -4;
	}

	while(true){
		addrLen = sizeof(clientAddr);
		clientSocket = (SOCKET)conn_accept((conn)m_ListenSock,(SOCKADDR *)&clientAddr,&addrLen);
		//clientSocket = accept(m_ListenSock,(SOCKADDR *)&clientAddr,&addrLen);
		if (clientSocket == NULL){

			CLog::Log(LOG_LEVEL_WARNING,"Accept Client Connect Error.\n");
			return -6;
		}

#if 0
		BOOL b = TRUE;
		int opt=sizeof(b);
		setsockopt(clientSocket,SOL_SOCKET,SO_KEEPALIVE,(char *)&b, opt);      
		getsockopt(clientSocket,SOL_SOCKET,SO_KEEPALIVE, (char *)&b, &opt);      
		CLog::Log(LOG_LEVEL_DEBUG,"getsockopt %d %d\n", opt, b);

		tcp_keepalive ka, ka2;
		ka.onoff = 1 ;
		ka.keepalivetime = 30000 ; // Keep Alive in 5.5 sec.
		ka.keepaliveinterval = 3000 ; // Resend if No-Reply
		if(WSAIoctl(clientSocket, SIO_KEEPALIVE_VALS, &ka, sizeof(ka), &ka2, sizeof(ka2), 
			(LPDWORD)&opt, NULL, NULL) != 0)
    		CLog::Log(LOG_LEVEL_WARNING,"setsockopt %d\n", WSAGetLastError());
		
		CLog::Log(LOG_LEVEL_DEBUG,"getsockopt %d %d\n", ka2.keepaliveinterval, ka2.keepalivetime);
#endif

		m_nCurrentThread +=1 ;

		//CLog::Log(LOG_LEVEL_WARNING,"Client Conn count is:%d\n",m_nCurrentThread);
		char ip[16];
		int port;
		strncpy(ip, inet_ntoa(clientAddr.sin_addr), 16);
		port = ntohs(clientAddr.sin_port);
		CLog::Log(LOG_LEVEL_NOMAL,"Incoming a connection %p [%s:%d]\n", (conn)clientSocket, ip, port);
		
		//处理用户登录请求
		//用户登录成功，创建线程处理用户业务，用户登录失败拒绝该连接
		INT cmdheader = sizeof(control_header);
		unsigned char cmd;
		short status;
		BYTE recvBuf[4096];
		int m = Read(clientSocket, &cmd, &status, recvBuf, sizeof(recvBuf));
		if(m <= 0 || cmd != CMD_LOGIN)
		{
			CLog::Log(LOG_LEVEL_WARNING, "[%s:%d] Invalid client, close it\n", ip, port);
			closesocket(clientSocket);
			continue;
		}

		CClientInfo* client;
		m = g_CrackBroker.ClientLogin2(recvBuf, ip, port, clientSocket, &client);
		Write(clientSocket, CMD_LOGIN, m, NULL,0,true);
		if(m != 0)
		{
			CLog::Log(LOG_LEVEL_WARNING, "[%s:%d] Authentication failure, close it\n", ip, port);
			closesocket(clientSocket);
			continue;
		}

		CLog::Log(LOG_LEVEL_NOTICE, "[%s:%d] Authentication OK, enjoy it\n", ip, port);
		
		hThread = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(&DispatchThread), client, 0, NULL);
		if (hThread == 0){
			CLog::Log(LOG_LEVEL_WARNING,"Create Thread Process Client Connection Error.\n");
			closesocket(clientSocket);
			continue;
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