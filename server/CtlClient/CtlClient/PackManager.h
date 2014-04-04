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

	//���ݷ����߳�
	pthread_t m_ThreadSend;	//�����������ݵ��߳̾��
	int m_bThreadSendStop;	//ֹͣ�̱߳��
	int m_bThreadSendRunning;//�߳����б��

	unsigned char *m_pSend;//���͵����� 
	int m_bToSend;//�����ݷ���
	int m_lenSend;//���ݳ���

	unsigned char *m_pRecv;//���յ����ݵ�ָ��
	int m_bRecved;//���յ�����
	int m_lenRecv;//

	static void *ThreadSend(void *);	//�����������ݵ��߳�
	void StartSend(void);				//�����������ݵ��߳�
	void StopSend(void);				//ֹͣ�������ݵ��߳�

	void *SendDataViaThread(void *);//�������ݽӿ�


	//�����̣߳�����������
	pthread_t m_ThreadHeartBeat;//�߳̾��
	int m_bThreadHeartBeatStop;	//ֹͣ�̱߳��
	int m_bThreadHeartBeatRunning;//�߳����б��

	static void *ThreadHeartBeat(void *);	//������
	void StartHeartBeat(void);				//���������߳�
	void StopHeartBeat(void);				//ֹͣ�����߳�
};

extern CPackManager g_packmanager;
