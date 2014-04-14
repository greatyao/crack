#pragma once

#include <afxmt.h>
#include <vector>
#include <map>
#include <deque>

#include "ClientInfo.h"
#include "CrackTask.h"
#include "CLog.h"
#include "ReqPacket.h"
#include "ResPacket.h"
#include "macros.h"



#define STANDARD_ERR -1000

#define CREATE_TASK_ERR STANDARD_ERR+1
#define START_TASK_ERR STANDARD_ERR+2
#define STOP_TASK_ERR STANDARD_ERR+3
#define DEL_TASK_ERR STANDARD_ERR+4
#define PAUSE_TASK_ERR STANDARD_ERR+5
#define GET_TASK_RES_ERR STANDARD_ERR+6
#define GET_TASK_STATUS_ERR	STANDARD_ERR+7
#define GET_CLIENT_ERR	STANDARD_ERR+8

#define NOT_FIND_GUID_TASK STANDARD_ERR+100

#define ALLOC_TASK_RESULT_ERR STANDARD_ERR+99
#define ALLOC_TASK_STATUS_ERR STANDARD_ERR+98


#define ALLOC_COMP_CLIENT_ERR	STANDARD_ERR+97

#define NOT_FINE_GUID_IN_QUEUE	STANDARD_ERR+96


#define ALLOC_CRACK_BLOCK_ERR	STANDARD_ERR+95

#define NO_RUNNING_TASK	STANDARD_ERR+94

#define NOT_FIND_GUID_BLOCK STANDARD_ERR+93

#define TASK_SPLIT_ERR	STANDARD_ERR+92

#define NOT_READY_WORKITEM STANDARD_ERR+91

struct MapLessCompare{
	bool operator()(const char * str1,const char *str2) const
	{
		return strcmp(str1,str2) < 0;
	}
};

//typedef std::map<char *, CCrackBlock *,MapLessCompare > TOTAL_CB_MAP;

typedef std::vector<CClientInfo *> CI_VECTOR;

typedef std::map<char *,CCrackTask *,MapLessCompare> CT_MAP;

//typedef std::queue<char *> CT_QUEUE;
typedef std::deque<char *> CT_DEQUE;

class CCrackBroker
{
public:
	CCrackBroker(void);
	~CCrackBroker(void);

	int ClientLogin(client_login_req *pReq);

	int ClientKeepLive(char *ip);
	int ClientKeepLive2(const char *ip, void* s, unsigned char* cmd, void** data);

	//控制节点业务逻辑处理函数

	//并未对任务进行切分
	int	CreateTask(struct crack_task *pReq,unsigned char *pguid);

	//文件上传成功后，对任务进行切分
	int SplitTask(char *pguid);

	int	StartTask(struct task_start_req *pReq);
	
	int StopTask(struct task_stop_req *pReq);
	int DeleteTask(struct task_delete_req *pReq);

	int PauseTask(struct task_pause_req *pReq);
	int GetTaskResult(struct task_result_req *pReq,struct task_result_info **pRes,int *resNum);

	int GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum);
	int GetClientList(struct compute_node_info **pRes,unsigned int *resNum);

	
	//计算节点业务逻辑处理函数
	int GetAWorkItem(struct crack_block **pRes);

	int GetWIStatus(struct crack_status *pReq);

	int GetWIResult(struct crack_result *pReq);

	int QueryTaskByWI(char* task_guid, const char* block_guid);

	
	int DoClientQuit(char *ip,int port);


	int deleteTask(char *guid);	//由于切分任务失败，删除刚创建的任务


	void *Alloc(int size);
	void Free(void *p);

private:
	int GetComputeNodesNum();
	int removeFromQueue(unsigned char *guid);

	int getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes);	//返回单个结果
	int	getResultFromTaskNew(CCrackTask *pCT,struct task_result_info *pRes); //返回多个结果

	
	int getStatusFromTask(CCrackTask *pCT,struct task_status_info *pRes);

	int getBlockFromCrackBlock(CCrackBlock *pCB,struct crack_block *pRes);

	void updateReadyQueue(CCrackBlock *pCB);

	void checkReadyQueue(CCrackTask *pCT);

public :
	
	CCriticalSection m_cracktask_cs;
	
	//方便快速查找任务
	CT_MAP m_cracktask_map;

	//循环队列，方便任务调度
	CT_DEQUE m_cracktask_ready_queue;

	CCriticalSection m_client_cs;
	CI_VECTOR m_client_list;

	CCriticalSection m_total_crackblock_cs;

	CB_MAP m_total_crackblock_map;

};


