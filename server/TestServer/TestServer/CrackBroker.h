#pragma once

#include "TestServer.h"
#include "ReqPacket.h"
#include "ResPacket.h"
#include "macros.h"

class CCrackTask;

class CCrackBroker
{
public:
	CCrackBroker(void);
	~CCrackBroker(void);

	int LoadFromPersistence(bool use_leveldb = true);

	//��½
	int ClientLogin2(const void* data, const char* ip, int port, unsigned int sock, CClientInfo ** res);

	//keepalive
	int ClientKeepLive2(const char *owner, void* s, unsigned char* cmd, void** data);

	//�˳�
	int DoClientQuit(const char *ip,int port);

	/////////////////////////////////////////////////////////////////////////
	//////////////////////	���ƽڵ�ҵ���߼������� /////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//��δ����������з�
	int	CreateTask(struct crack_task *pReq, void* pclient);

	//�ļ��ϴ��ɹ��󣬶���������з�
	int SplitTask(const char *guid, const char* john = NULL);

	int	StartTask(struct task_start_req *pReq, void* pclient);
	
	int StopTask(struct task_stop_req *pReq, void* pclient);
	int DeleteTask(struct task_delete_req *pReq, void* pclient);

	int PauseTask(struct task_pause_req *pReq, void* pclient);
	int GetTaskResult(struct task_result_req *pReq,struct task_result_info **pRes,int *resNum, void* pclient);

	int GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum, void* pclient);
	int GetClientList(struct compute_node_info **pRes,unsigned int *resNum);

	int deleteTask(const char *guid, void* pclient);	//�����з�����ʧ�ܣ�ɾ���մ���������

	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////	����ڵ�ҵ���߼������� /////////////////////////
	/////////////////////////////////////////////////////////////////////////
	
	int GetAWorkItem2(const char *worker, struct crack_block **pRes); //���Ӵ����������ڵ�ip��ַ�Ͷ˿�

	int GetWIStatus(struct crack_status *pReq);

	int GetWIResult(struct crack_result *pReq);

	int QueryTaskByWI(char* task_guid, const char* block_guid);

	/////////////////////////////////////////////////////////////////////////
	//////////////////////		�ڴ洦����		/////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void *Alloc(int size);
	void Free(void *p);
	
	static void GetTaskFileByGuid(const char* guid, char* fn, int n);

private:
	friend class CCrackTask;

	int GetComputeNodesNum();
	int removeFromQueue(const char *guid);

	int getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes);	//���ص������
	int	getResultFromTaskNew(CCrackTask *pCT,struct task_result_info *pRes); //���ض�����

	
	int getStatusFromTask(CCrackTask *pCT,struct task_status_info *pRes);

	int getBlockFromCrackBlock(CCrackBlock *pCB,struct crack_block *pRes);

	void updateReadyQueue(CCrackBlock *pCB);

	void checkReadyQueue(CCrackTask *pCT);


	///add the block<---->computer op
	//int addNewCompBlock(char *ipinfo,char *blockguid,char status);
	int deleteCompBlock(const char *worker,char *blockguid);
	int setCompBlockStatus(const char *worker,char *blockguid,char status);

	int getBlockByComp(const char *worker,CBN_VECTOR& cbnvector,char status);

	int setNoticByHash(CCrackBlock *pCB,int index);

	typedef struct ngx_pool_s  ngx_pool_t;
	ngx_pool_t* m_pool;

public :
	
	//CCriticalSection m_cracktask_cs;
	
	//������ٲ�������
	CT_MAP m_cracktask_map;

	//ѭ�����У������������
	CT_DEQUE m_cracktask_ready_queue;

	//CCriticalSection m_client_cs;
	CI_VECTOR m_client_list;

	//CCriticalSection m_total_crackblock_cs;

	CB_MAP m_total_crackblock_map;

	//����ڵ��block ��ӳ��
	CCB_MAP m_comp_block_map;

};

extern CCrackBroker g_CrackBroker;

