#pragma once

#include "Common.h"
#include "guidgenerator.h"
#include "ClientRequest.h"
#include "Broker.h"
#include "TaskManager.h"
#include "WorkItemManager.h"
#include "SimpleTaskSchedule.h"

#include "CLog.h"


class CBrokerManager : public CTaskManager
{

public:
	CBroker serverBroker;

public:
	CBrokerManager(void);
	~CBrokerManager(void);

	CTask * GetTaskByGuid(GUID guid);
	CTask * GetTaskByGuid(std::string guid);
	CTask * GetTaskByStatus(BYTE status);
	
	
	//处理控制节点请求
	std::string ReqUploadTask(CtlTaskUpload *ptaskupload);  //处理任务上传请求
	
	//处理任务开始请求
	INT	ReqStartTask(CtlTaskStart *ptaskstart);
	
	//处理任务停止请求
	INT ReqStopTask(CtlTaskGuid *ptaskguid);
	
	//处理任务删除请求
	INT ReqDelTask(CtlTaskGuid *ptaskguid);
	

	//处理任务暂停请求
	INT ReqPauseTask(CtlTaskGuid *ptaskguid);

	//获取任务执行结果
	TaskResult * ReqGetTaskResult(CtlTaskGuid *ptaskguid);

	//获取正在解密任务状态
	TaskStatus *ReqGetTaskStatus();

	//获取任务计算节点信息
	CompNodeInfo *ReqGetClientList();

	//处理计算节点请求

	//处理解密成功完成请求
	WorkItemGuid * ReqDecRecovered(CompWIRecovered *pwirec);

	//处理解密完成，单位找到结果
	WorkItemGuid * ReqDecUnRecovered(CompWIUnRecovered *pwiunrec);

	//请求工作项任务
	WorkItemInfo * ReqWorkitems(CompGuid *pguid);
	

	//处理解密失败
	INT ReqDecFailure(CompWIFailed *pwifailed);
		
	//CTask * CreateWorkItem();	//创建工作项



	//任务切分算法
	int splitByCondition();


	//任务调度算法,从一个任务列表中选择待执行的任务
	CTask * taskSchedule(std::vector<CTask *> tlist);

	//动态开辟空间接口
	LPVOID _Alloc(UINT size);
	VOID _Free(LPVOID pt);

};
