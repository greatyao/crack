#include "SockServer.h"
#include "ServerResp.h"
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
		return -1;
	}
	
	CLog::Log(LOG_LEVEL_WARNING,"Init Server Socket Lib OK\n");

	m_ListenSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_ListenSock == INVALID_SOCKET){
		
		CLog::Log(LOG_LEVEL_WARNING,"Create Server Socket Error.\n");
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
		CLog::Log(LOG_LEVEL_WARNING,"Incoming a new client [%s:%d]\n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
		//处理用户登录请求
	

		//用户登录成功，创建线程处理用户业务，用户登录失败拒绝该连接
		




		SOCKET* s = new SOCKET(clientSocket);
		hThread = CreateThread(NULL,0,reinterpret_cast<LPTHREAD_START_ROUTINE>(&DispatchThread),(LPVOID)s,0,NULL);
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

/*

int CSockServer::clientlogin(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	char buf[40];
	char ip[20];
	int port = 0;

	client_login_req *pC = (client_login_req *)pdata;
	client_login_req myclient;

	getClientIPInfo(pclient,ip,&port);

	if (pdata == NULL){
		
		memset(&myclient,0,sizeof(client_login_req));
		memset(buf,0,40);
		sprintf(buf,"%u",*(SOCKET *)pclient);
		myclient.m_clientsock = *(SOCKET *)pclient;
		myclient.m_cputhreads = myclient.m_gputhreads = 0;
		myclient.m_type = 0;
		memcpy(myclient.m_ip,ip,16);

		pC = &myclient;
		
	}

	pC->m_port = port;
	pC->m_clientsock = *(SOCKET *)pclient;
	memcpy(pC->m_ip,ip,strlen(ip));
	//处理业务逻辑

	nRet = g_CrackBroker.ClientLogin(pC);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Client Login IP %s port %d Error\n",ip,port);
	}else {
		//CLog::Log(LOG_LEVEL_WARNING,"Client Login IP %s port %d OK\n",ip,port);
	}
	////////////////////////////////////

	//产生应答报文，并发送

	int m = Write(*(SOCKET*)pclient, CMD_LOGIN, 0, NULL,0,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Login :Send Response Error %d \n",ip,port,m);
		
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Login OK\n",ip,port);
	}

	return nRet;
}

*/