#include "StdAfx.h"
#include "BrokerManager.h"

CBrokerManager::CBrokerManager(void)
{
}

CBrokerManager::~CBrokerManager(void)
{
}



CTask * CBrokerManager::GetTaskByGuid(GUID guid){

	std::string str_guid;
	CTask *ptask = NULL;
	CTask *pTmpTask = NULL;

	int task_count = serverBroker.m_coretask_list.size();
	int i = 0;
	
	guid_to_string(&guid,str_guid);
	for(i = 0; i < task_count; i++){

		pTmpTask = serverBroker.m_coretask_list[i];	
		
		if (pTmpTask->m_string_ct_guid.compare(str_guid) == 0)
			ptask = pTmpTask;
			break;

	}
	

	return ptask;
}

CTask * CBrokerManager::GetTaskByGuid(std::string str_guid){


	CTask *ptask = NULL;
	CTask *pTmpTask = NULL;
	int task_count = serverBroker.m_coretask_list.size();
	int i = 0;
	
	guid_to_string(&guid,str_guid);
	for(i = 0; i < task_count; i++){

		pTmpTask = serverBroker.m_coretask_list[i];	
		
		if (pTmpTask->m_string_ct_guid.compare(str_guid) == 0)
			ptask = pTmpTask;
			break;

	}

	return ptask;
}

//获得指定状态的任务列表,直接在处理函数中完成
CTask * CBrokerManager::GetTaskByStatus(BYTE status){

	/*
	CTask *ptask = NULL;
	CTask *ptasks = NULL;

	int task_count = serverBroker.m_coretask_list.size();
	int i = 0;
	int num = 0;

	for(i = 0; i < task_count; i++){

		ptask = serverBroker.m_coretask_list[i];	
		
		if (ptask->m_task_status == status){
			num ++;
		}
	}
	*/

	return NULL;
}


//处理控制节点请求
//处理任务上传请求
string CBrokerManager::ReqUploadTask(CtlTaskUpload *ptaskupload){
	
	
	CTask *ptask = CreateTask();
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Error\n");
		return NULL;

	}

	ptask->m_algtype = ptaskupload->m_algorithm;
	ptask->m_hashinfo = ptaskupload->m_hashinfo;
	ptask->m_ctlguid = ptaskupload->m_ctlguid;

	serverBroker.m_coretask_list.push_back(ptask);
	return ptask->m_string_ct_guid;
}


/*
int CBrokerManager::splitByCondition(){


	return 3;
}
*/


//处理任务开始请求
INT	CBrokerManager::ReqStartTask(CtlTaskStart *ptaskstart){

	INT nRet = 0;
	int wi_count = 0;
	CTask *ptask = NULL;
	CWorkItem *pwi= NULL;
	
	ptask = GetTaskByGuid(ptaskstart->m_guid);

	//split the task by the ptaskstart conditions
	//wi_count = splitByCondition();
	for (int i = 0;i < wi_count ;i++){

		pwi = new CWorkItem;
		if (!pwi){
			
			CLog::Log(LOG_LEVEL_WARNING,"Start Task ,Work Item Create Failure\n");
			nRet = -1;
		}
		
	}




	return nRet;

}

	
//处理任务停止请求
INT CBrokerManager::ReqStopTask(CtlTaskGuid *ptaskguid){

	INT nRet = 0;
	CTask *ptask = NULL;
	ptask = GetTaskByGuid(ptaskguid->m_guid);
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task Failure\n");
		nRet = -1;
	}else{

		//ptask->m_task_status = CT_TASK_READY;
		nRet = StopTask(ptask);
	}

	return nRet;
}
	
//处理任务删除请求
INT CBrokerManager::ReqDelTask(CtlTaskGuid *ptaskguid){

	INT nRet = 0;
	CTask *ptask = NULL;
	ptask = GetTaskByGuid(ptaskguid->m_guid);
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Delete Task Failure\n");
		nRet = -1;
	}else{

		//ptask->m_task_status = CT_TASK_READY;
		nRet = DeleteTask(ptask);
	}
	return nRet;
}
	

//处理任务暂停请求
INT CBrokerManager::ReqPauseTask(CtlTaskGuid *ptaskguid){

	INT nRet = 0;
	CTask *ptask = NULL;
	ptask = GetTaskByGuid(ptaskguid->m_guid);
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task Failure\n");
		nRet = -1;
	}else{

		//ptask->m_task_status = CT_TASK_READY;
		nRet = PauseTask(ptask);
	}
	return nRet;

}

//获取任务执行结果
TaskResult * CBrokerManager::ReqGetTaskResult(CtlTaskGuid *ptaskguid){
	
	TaskResult *pTaskResult = NULL;
	INT nRet = 0;
	CTask *ptask = NULL;
	ptask = GetTaskByGuid(ptaskguid->m_guid);
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Get Task By guid Failure\n");
		nRet = -1;
	}else{

/*
	unsigned char m_result[DEC_RESULT_SIZE];
	unsigned char m_dec_status;

}TaskResult;
	
typedef enum TASK_STATUS{

	//添加了任务状态
	CT_STATUS_READY = 1,   //任务的就绪状态
	CT_STATUS_FINISHED,		//任务完成状态	
	CT_STATUS_FAILURE,		//任务解密失败状态
	CT_STATUS_RUNNING,		//任务正在解密状态
	CT_STATUS_PAUSED,		//任务暂停解密状态


	CT_STATUS_DELETED,		//将任务设置为删除状态
	CT_STATUS_MAX

};
*/

		pTaskResult = _Alloc(sizeof(TaskResult));
		if (!pTaskResult){
			
			CLog::Log(LOG_LEVEL_WARNING,"Get Task Result Error\n");
			return NULL;

		}


		switch(ptask->m_task_status){

			case CT_STATUS_FINISHED:
				
				CopyMemory(pTaskResult->m_result,ptask->m_string_result.c_str(),strlen(ptask->m_string_result.c_str()));
				pTaskResult->m_dec_status = ptask->m_task_status;
				break;
			default:
				pTaskResult->m_dec_status = ptask->m_task_status;
				break;
		}
		
	}

	return pTaskResult;

}

//获取正在解密任务状态
TaskStatus *CBrokerManager::ReqGetTaskStatus(){

	BYTE task_status = CT_STATUS_RUNNING;
	GUID guid;
	CTask *ptask = NULL;
	
	TaskStatus *pTaskStatus = NULL;

	int task_count = serverBroker.m_coretask_list.size();
	int i = 0;
	int num = 0;

	for(i = 0; i < task_count; i++){

		ptask = serverBroker.m_coretask_list[i];	
		
		if (ptask->m_task_status == task_status){
				num ++;		

		}
	}

/*
	GUID guid;
	float m_progress;
	
	unsigned m_split_number;
	unsigned m_fini_number;
	
	//......

}TaskStatus;
*/
	pTaskStatus = _Alloc(num * sizeof(TaskStatus));
	if (!pTaskStatus){

		CLog::Log(LOG_LEVEL_WARNING,"create task status memory failure\n");
	}else{

		num = 0;
		for(i = 0; i < task_count; i++){

			ptask = serverBroker.m_coretask_list[i];	
			
			if (ptask->m_task_status == task_status){
						
				string_to_guid(ptask->m_string_ct_guid,&guid);

				CopyMemory(&pTaskStatus[num].guid,&guid,sizeof(GUID));
				pTaskStatus[num].m_progress = ptask->m_progress;
				pTaskStatus[num].m_split_number = ptask->m_split_number;
				pTaskStatus[num].m_fini_number = ptask->m_finished_number;
				num++;

			}
		}

	}

	return pTaskStatus;
}

//获取任务计算节点信息 已有CClientManager 实现
CompNodeInfo *CBrokerManager::ReqGetClientList(){


	return NULL;
}

//处理计算节点请求

//处理工作项解密成功完成请求,通过工作项查询任务，任务状态变为完成，经相关结果保存，
//修改其他工作项的状态
WorkItemGuid * CBrokerManager::ReqDecRecovered(CompWIRecovered *pwirec){

	INT nRet = 0;
	CWorkItem *pwi = NULL;
	CTask *pTask = NULL;
	WorkItemGuid *pwiguid = NULL;

	GUID guid;
	int task_size = 0;
	int i = 0;
	std::vector<CTask *> tlist = serverBroker.m_coretask_list;

	task_size = tlist.size();
	for (i = 0 ;i < task_size ;i ++ ){
		
		pTask = tlist[i];
		
		pwi = GetWorkItemByGuid(pTask,pwirec->m_guid);
		if (pwi){
				
			UpdateStatusFinished(pwi);
			RecoveredTask(pTask,pwirec->m_result);
			break;
		}
		
	}
	
	if (i == task_size){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find WorkItem By Guid\n");
		return NULL;

	}

	
	string_to_guid(pwirec->m_guid,&guid);
	pwiguid = _Alloc(sizeof(WorkItemGuid));
	if (!pwiguid){

		CLog::Log(LOG_LEVEL_WARNING,"WI Recovered :Alloc Workitem Guid Error\n");

		
	}else{

		CopyMemory(&pwiguid->guid,&guid,sizeof(GUID));
		CLog::Log(LOG_LEVEL_WARNING,"WI Recovered :Alloc Workitem Guid OK\n");
	}

	
	return pwiguid;

}

//处理工作项解密完成，但未找到结果
WorkItemGuid * CBrokerManager::ReqDecUnRecovered(CompWIUnRecovered *pwiunrec){

	INT nRet = 0;
	CWorkItem *pwi = NULL;
	CTask *pTask = NULL;
	WorkItemGuid *pwiguid = NULL;
	GUID guid;
	int task_size = 0;
	int i = 0;
	std::vector<CTask *> tlist = serverBroker.m_coretask_list;

	task_size = tlist.size();
	for (i = 0 ;i < task_size ;i ++ ){
		
		pTask = tlist[i];
		
		pwi = GetWorkItemByGuid(pTask,pwiunrec->m_guid);
		if (pwi){
			
			UpdateStatusNoPWD(pwi);  //该工作项执行结束，未找到密码
			UnRecoveredTask(pTask);		//修改任务相关状态
			break;
		}
		
	}

	if (i == task_size){
		
		CLog::Log(LOG_LEVEL_WARNING,"WI UnRecovered :Can't find WorkItem By Guid\n");
		return NULL;

	}

	string_to_guid(pwiunrec->m_guid,&guid);
	pwiguid = _Alloc(sizeof(WorkItemGuid));
	if (!pwiguid){

		CLog::Log(LOG_LEVEL_WARNING,"WI UnRecovered :Alloc Workitem Guid Error\n");
		
	}else{

		CopyMemory(&pwiunrec->guid,&guid,sizeof(GUID));
		CLog::Log(LOG_LEVEL_WARNING,"WI UnRecovered :Alloc Workitem Guid OK\n");
	}

	return pwiguid;
}

//请求工作项任务
WorkItemInfo * CBrokerManager::ReqWorkitems(CompGuid *pguid){


	CTaskSchedule *pSchedule = NULL;
	WorkItemInfo *pWInfo = NULL;
 	CTask *ptask = NULL;
	CWorkItem *pwi = NULL;
	INT nRet = 0;
	int size = 0;
	int i = 0;
	std::vector<CTask *> tlist = serverBroker.m_coretask_list;
	size = tlist.size();

	pSchedule = new CSimpleTaskSchedule;

	ptask = pSchedule->Schedule(tlist);

	delete pSchedule;

	if (!ptask){
		
		CLog::Log(LOG_LEVEL_WARNING,"No Schedule Usefull Task Error\n");
		
	}else{

		pWInfo = _Alloc(sizeof(WorkItemInfo));
		if (!pWInfo){

			CLog::Log(LOG_LEVEL_WARNING,"Request Work item error\n");

		}else{
/*
	GUID guid;
	BYTE wi_john[128];
	BYTE wi_begin[16];
	BYTE wi_end[16];
	BYTE wi_charset[16];
	
}WorkItemInfo;

std::string m_string_wi_guid;	//workitem的guid

	std::string m_string_wi_john;	//目标破解的信息
	std::string m_string_wi_begin;	//暴力破解的开始字符
	std::string m_string_wi_end;	//暴力破解的结束字符
	std::string m_string_wi_chars_set;//暴力破解的字符集

	unsigned char m_wi_status;	//workitem当前状态
	std::string m_comp_guid; //分配的计算节点guid

*/
			pwi = GetReadyWI(ptask);
			if (!pwi){

				CLog::Log(LOG_LEVEL_WARNING,"Get Ready Work item error\n");
			}else{
				
				UpdateStatusRuning(pwi);
				string_to_guid(pwi->m_string_wi_guid,&(pWInfo->guid));
				strcpy(pWInfo->wi_john,pwi->m_string_wi_john.c_str());
				strcpy(pWInfo->wi_begin,pwi->m_string_wi_begin.c_str());
				strcpy(pWInfo->wi_end,pwi->m_string_wi_end.c_str());
				strcpy(pWInfo->wi_charset,pwi->m_string_wi_chars_set.c_str());
		
			}

		}

	}
	return pWInfo;

}
	

//处理工作项解密失败
INT CBrokerManager::ReqDecFailure(CompWIFailed *pwifailed){

	INT nRet = 0;
	CWorkItem *pwi = NULL;
	CTask *pTask = NULL;
	int task_size = 0;
	int i = 0;
	std::vector<CTask *> tlist = serverBroker.m_coretask_list;

	task_size = tlist.size();
	for (i = 0 ;i < task_size ;i ++ ){
		
		pTask = tlist[i];
		
		pwi = GetWorkItemByGuid(pTask,pwifailed->m_wi_guid);
		if (pwi){

			UpdateStatusFailure(pwi); //工作项执行失败
			FailedTask(pTask);
			break;
			
		}
		
	}

	if (i == task_size){
		
		CLog::Log(LOG_LEVEL_WARNING,"WI Failure :Can't find WorkItem By Guid\n");
		return -1;

	}
	return 0;

}



LPVOID CBrokerManager::_Alloc(UINT size){

	
	LPBYTE p = NULL;

	p = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);
	
	return (LPVOID)p;

}

VOID CBrokerManager::_Free(LPVOID pt){

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pt);

}






