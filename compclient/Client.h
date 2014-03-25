/* Client.h
 *
 * Client for connection with server
 * Copyright (C) 2014 TRIMPS
 *
 * Created By YAO Wei at  03/19/2014
 */
 
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <pthread.h>

struct crack_status;
struct crack_result;
struct crack_block;

class Client
{
public:
	static Client& Get();
	
	//销毁资源
	void Destory();
	
	int Connect(const char* ip, unsigned short port);
	
	int WriteToServer(const void* data, int size);
	
	//从服务端获取workitem
	int GetWorkItemFromServer(crack_block* item);
	
	//通报服务端解密状态
	int ReportStatusToServer(crack_status* status);
	
	//通报服务端解密解密结果
	int ReportResultToServer(crack_result* result);
	
	static void* MonitorThread(void* p);
	
private:
	Client();
	~Client();
	
	int Read(unsigned char *cmd, short* status, void* data, int size);
	
	int Write(unsigned char cmd, const void* data, int size);

private:
	int sck;
	char ip[16];
	unsigned short port;
	int connected;//0表示断开，1表示正在连，2表示连接上了
	bool stop;
	pthread_t tid;
	pthread_mutex_t mutex;
};

#endif