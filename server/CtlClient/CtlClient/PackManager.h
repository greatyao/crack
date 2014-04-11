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

#define LOAD_FILE_ERROR -300
#define SPLIT_BLOCK_ERROR -301
#define DELET_TASK_ERROR -100

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

	int GenTaskStatusPack(task_status_info **res);

	int GenClientStatusPack(compute_node_info **res);

	//修改获得任务结果的协议
	int GenTaskResultPack(task_result_req req,task_result_info **res);


	//添加新的文件上传协议处理接口

	//处理文件上传
	int GenNewFileUploadPack(file_upload_req req,file_upload_res *res);

	//处理文件上传开始
	int GenNewFileUploadStartPack(file_upload_start_res *res);

	//文件传输
	int GenNewFileUploadingPack();

	//处理文件上传结束
	int GenNewFileUploadEndPack(file_upload_end_res *res);



	void GetErrMsg(short status,char *msg);

public:
	char m_cur_upload_guid[40];
	void * m_cur_server_file;
	unsigned int m_cur_upload_file_len;
	char m_cur_local_file[256];
	void * m_file_desc;
	CSocketClient m_sockclient;


	//增加线程，处理心跳包
	HANDLE m_hStopSleep;
	HANDLE CreateSleep(void);
	void StartSleep(HANDLE hHandle,unsigned long dwMilliseconds);
	void StopSleep(HANDLE hHandle);

	pthread_t m_ThreadHeartBeat;//线程句柄
	int m_bThreadHeartBeatStop;	//停止线程标记
	int m_bThreadHeartBeatRunning;//线程运行标记

	static void *ThreadHeartBeat(void *);	//心跳包
	void StartHeartBeat(void);				//启动心跳线程
	void StopHeartBeat(void);				//停止心跳线程

	//处理和服务器端的连接	
	int m_connected;
	int StartClient(void);
	int StopClient(void);

	//内存分配函数
	void * _malloc(size_t size){return malloc(size);}
	void _free(void *mem){free(mem);}

	//互斥
	CRITICAL_SECTION m_csSocket;
	void InitLockSocket(void);
	void DelLockSocket(void);
	void LockSocket(void);
	void UnLockSocket(void);
};

extern CPackManager g_packmanager;
