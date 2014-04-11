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

	//�޸Ļ����������Э��
	int GenTaskResultPack(task_result_req req,task_result_info **res);


	//����µ��ļ��ϴ�Э�鴦��ӿ�

	//�����ļ��ϴ�
	int GenNewFileUploadPack(file_upload_req req,file_upload_res *res);

	//�����ļ��ϴ���ʼ
	int GenNewFileUploadStartPack(file_upload_start_res *res);

	//�ļ�����
	int GenNewFileUploadingPack();

	//�����ļ��ϴ�����
	int GenNewFileUploadEndPack(file_upload_end_res *res);



	void GetErrMsg(short status,char *msg);

public:
	char m_cur_upload_guid[40];
	void * m_cur_server_file;
	unsigned int m_cur_upload_file_len;
	char m_cur_local_file[256];
	void * m_file_desc;
	CSocketClient m_sockclient;


	//�����̣߳�����������
	HANDLE m_hStopSleep;
	HANDLE CreateSleep(void);
	void StartSleep(HANDLE hHandle,unsigned long dwMilliseconds);
	void StopSleep(HANDLE hHandle);

	pthread_t m_ThreadHeartBeat;//�߳̾��
	int m_bThreadHeartBeatStop;	//ֹͣ�̱߳��
	int m_bThreadHeartBeatRunning;//�߳����б��

	static void *ThreadHeartBeat(void *);	//������
	void StartHeartBeat(void);				//���������߳�
	void StopHeartBeat(void);				//ֹͣ�����߳�

	//����ͷ������˵�����	
	int m_connected;
	int StartClient(void);
	int StopClient(void);

	//�ڴ���亯��
	void * _malloc(size_t size){return malloc(size);}
	void _free(void *mem){free(mem);}

	//����
	CRITICAL_SECTION m_csSocket;
	void InitLockSocket(void);
	void DelLockSocket(void);
	void LockSocket(void);
	void UnLockSocket(void);
};

extern CPackManager g_packmanager;
