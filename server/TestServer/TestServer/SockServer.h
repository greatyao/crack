#pragma once


class CSockServer
{
public:
	CSockServer(void);
	~CSockServer(void);

	SOCKET m_ListenSock;
	UINT m_nMaxThread;			//最大处理线程数
	UINT m_nCurrentThread;		//当前处理线程数
	UINT m_nPort;				//当前监听端口
	UINT m_nMaxConnNum;
	

	//服务器初始化，参数：监听端口、允许最大连接
	UINT Initialize(UINT port,UINT maxConnNum);




	INT StartServer(void);
	INT ShutDown(void);

	
};


