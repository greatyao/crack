#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <pthread.h>

class Client
{
public:
	static Client& Get();
	
	int Connect(const char* ip, unsigned short port);
	
	int WriteToServer(const void* data, int size);
	
	int GetWorkItemFromServer(void* data, int size);
	
	static void* MonitorThread(void* p);
	
private:
	Client();
	~Client();
	
	int Read(void* data, int size);
	
	int Write(const void* data, int size);

private:
	int sck;
	char ip[16];
	unsigned short port;
	int connected;//0表示断开，1表示正在连，2表示连接上了
	pthread_t tid;
	pthread_mutex_t mutex;
};

#endif