#pragma once

class CSocketClient
{
public:
	CSocketClient(void);
	~CSocketClient(void);
	
	int Init(char *ip,int port);
	int Read(unsigned char  *cmd,short *status,void *data,int size);
	int Write(unsigned char cmd, short status,void *data,int size);
	int WriteNoCompress(unsigned char cmd, short status,void *data,int size);
	int mysend(void* buf, int size, int flag);

	int Finish();
public:
		

	SOCKET m_clientsocket;

};
