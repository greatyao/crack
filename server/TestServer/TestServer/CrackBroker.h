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

	//登陆
	int ClientLogin2(const void* data, const char* ip, int port, unsigned int sock, CClientInfo ** res);

	//keepalive
	int ClientKeepLive2(const char *owner, void* s, unsigned char* cmd, void** data);

	//退出
	int DoClientQuit(const char *ip,int port);

	/////////////////////////////////////////////////////////////////////////
	//////////////////////	控制节点业务逻辑处理函数 /////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//并未对任务进行切分
	int	CreateTask(struct crack_task *pReq, void* pclient);

	//文件上传成功后，对任务进行切分
	int SplitTask(const char *guid, const char* john = NULL);

	int	StartTask(struct task_start_req *pReq, void* pclient);
	
	int StopTask(struct task_stop_req *pReq, void* pclient);
	int DeleteTask(struct task_delete_req *pReq, void* pclient);

	int PauseTask(struct task_pause_req *pReq, void* pclient);
	int GetTaskResult(struct task_result_req *pReq,struct task_result_info **pRes,int *resNum, void* pclient);

	int GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum, void* pclient);
	int GetClientList(struct compute_node_info **pRes,unsigned int *resNum);

	int deleteTask(const char *guid, void* pclient);	//由于切分任务失败，删除刚创建的任务

	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////	计算节点业务逻辑处理函数 /////////////////////////
	/////////////////////////////////////////////////////////////////////////
	
	int GetAWorkItem2(const char *worker, struct crack_block **pRes); //增加传入参数计算节点ip地址和端口

	int GetWIStatus(struct crack_status *pReq);

	int GetWIResult(struct crack_result *pReq);

	int QueryTaskByWI(char* task_guid, const char* block_guid);

	/////////////////////////////////////////////////////////////////////////
	//////////////////////		内存处理函数		/////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void *Alloc(int size);
	void Free(void *p);
	
	static void GetTaskFileByGuid(const char* guid, char* fn, int n);

private:
	friend class CCrackTask;

	int GetComputeNodesNum();
	int removeFromQueue(const char *guid);

	int getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes);	//返回单个结果
	int	getResultFromTaskNew(CCrackTask *pCT,struct task_result_info *pRes); //返回多个结果

	
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
	
	//方便快速查找任务
	CT_MAP m_cracktask_map;

	//循环队列，方便任务调度
	CT_DEQUE m_cracktask_ready_queue;

	//CCriticalSection m_client_cs;
	CI_VECTOR m_client_list;

	//CCriticalSection m_total_crackblock_cs;

	CB_MAP m_total_crackblock_map;

	//计算节点和block 的映射
	CCB_MAP m_comp_block_map;

};

extern CCrackBroker g_CrackBroker;

