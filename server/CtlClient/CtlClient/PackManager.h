#pragma once
#include "ReqPacket.h"
#include "macros.h"
#include "algorithm_types.h"
#include "ResPacket.h"
#include "SocketClient.h"

//pthread
#include "pthread.h"
#include "sched.h"
#include "semaphore.h"

class CPackManager
{
public:
	CPackManager(void);
	~CPackManager(void);

	int DoLoginPack(client_login_req req);

	int DoKeeplivePack();

	int DoTaskUploadPack(crack_task req,task_upload_res *res);

	int GenTaskStartPack(task_start_req req,task_status_res *res);

	int GenTaskStopPack(task_stop_req req,task_status_res *res);

	int GenTaskPausePack(task_pause_req req,task_status_res *res);

	int GenTaskDeletePackt(task_delete_req req,task_status_res *res);

	int GenTaskResultPack(task_result_req req,task_status_res *res);

	int GenTaskStatusPack(task_status_info **res);

	int GenClientStatusPack(compute_node_info **res);

	int GenFileUploadPack(file_upload_req req,file_upload_res *res);

	int GenFileUploadStart(file_upload_end_res *res);

public:

	char m_cur_upload_guid[40];
	void * m_cur_server_file;
	unsigned int m_cur_upload_file_len;
	char m_cur_local_file[256];
	CSocketClient m_sockclient;

	//数据发送线程
	pthread_t m_ThreadSend;	//发送网络数据的线程句柄
	int m_bThreadSendStop;	//停止线程标记
	int m_bThreadSendRunning;//线程运行标记

	unsigned char *m_pSend;//发送的数据 
	int m_bToSend;//有数据发送
	int m_lenSend;//数据长度

	unsigned char *m_pRecv;//接收到数据的指针
	int m_bRecved;//接收到数据
	int m_lenRecv;//

	static void *ThreadSend(void *);	//发送网络数据的线程
	void StartSend(void);				//启动发送数据的线程
	void StopSend(void);				//停止发送数据的线程

	void *SendDataViaThread(void *);//发送数据接口


	//增加线程，处理心跳包
	pthread_t m_ThreadHeartBeat;//线程句柄
	int m_bThreadHeartBeatStop;	//停止线程标记
	int m_bThreadHeartBeatRunning;//线程运行标记

	static void *ThreadHeartBeat(void *);	//心跳包
	void StartHeartBeat(void);				//启动心跳线程
	void StopHeartBeat(void);				//停止心跳线程
};

extern CPackManager g_packmanager;
