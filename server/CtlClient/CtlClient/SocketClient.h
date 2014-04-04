#pragma once

class CSocketClient
{
public:
	CSocketClient(void);
	~CSocketClient(void);
	
	int Init(char *ip,int port);

	//返回读取的data的字节数，出错返回负数，0表示数据结束或者没有数据
	//如果数据量很大，一次性发送不完，则在seq中体现；何时判断数据结束?看read是否返回0
	int Read(unsigned char  *cmd,short *status,void *data,int size, unsigned int* seq = NULL);
	
	//如果数据量很小，一次性就发送完毕，则seq默认为0
	//否则必须让seq从1开始递增；最后必须发送一个dataLen为-1的数据，表明数据结束
	int Write(unsigned char cmd, short status,void *data,int size, unsigned int seq = 0);
	int WriteNoCompress(unsigned char cmd, short status,void *data,int size, unsigned int seq = 0);

	int Finish();
public:
		

	SOCKET m_clientsocket;

};
