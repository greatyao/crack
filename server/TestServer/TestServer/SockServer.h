#pragma once


class CSockServer
{
public:
	CSockServer(void);
	~CSockServer(void);

	SOCKET m_ListenSock;
	UINT m_nMaxThread;			//������߳���
	UINT m_nCurrentThread;		//��ǰ�����߳���
	UINT m_nPort;				//��ǰ�����˿�
	UINT m_nMaxConnNum;
	

	//��������ʼ���������������˿ڡ������������
	UINT Initialize(UINT port,UINT maxConnNum);




	INT StartServer(void);
	INT ShutDown(void);

	
};


