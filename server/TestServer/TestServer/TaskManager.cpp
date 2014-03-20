#include "StdAfx.h"
#include "TaskManager.h"
#include "CLog.h"


CTaskManager::CTaskManager(void)
{
}

CTaskManager::~CTaskManager(void)
{
}


CTask* CTaskManager::CreateTask(){

	CTask *pTask = new CTask;
	if (!pTask){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Error\n");
		pTask = NULL;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Create Task OK\n");
		
	}
	return pTask;

}

//启动任务，同时将任务划分为工作项，并初始化工作项
INT CTaskManager::StartTask(CTask *ptask){

	INT nRet = 0;
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Start Task error\n");
		nRet = -1;
	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Start Task OK\n");
		ptask->m_task_status = CT_STATUS_RUNNING;
		
		//......

		nRet = 0;
	}
	
	return nRet;
}

//停止任务，同时将相关的工作项设置停止状态
INT CTaskManager::StopTask(CTask *ptask){

	CWorkItem *pwi = NULL;
	INT size = 0;
	int i = 0;
	INT nRet = 0;
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task error\n");
		nRet = -1;
	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task OK\n");
		ptask->m_task_status = CT_STATUS_READY;

		std::vector<CWorkItem *> wilist = ptask->m_workitem_list;
		size = wilist.size();
		for(i =  0; i < size ;i ++ ){
			pwi = wilist[i];
			if ((pwi->m_wi_status == WI_STATUS_RUNNING ) || 
				(pwi->m_wi_status == WI_STATUS_WAITING)){
				pwi->m_wi_status = WI_STATUS_STOPPED;
			}
		}
		nRet = 0;
	}
	return nRet;

}

//删除任务，同时将相关的工作项设置为停止
INT CTaskManager::DeleteTask(CTask *ptask){

	CWorkItem *pwi = NULL;
	INT size = 0;
	int i = 0;
	INT nRet = 0;
	if (!ptask){


		CLog::Log(LOG_LEVEL_WARNING,"Delete Task error\n");
		nRet = -1;
	}else{

		//delete ptask;
		ptask->m_task_status = CT_STATUS_DELETED;  //将任务设置为删除状态
		std::vector<CWorkItem *> wilist = ptask->m_workitem_list;
		size = wilist.size();
		for(i =  0; i < size ;i ++ ){
			pwi = wilist[i];
			pwi->m_wi_status = WI_STATUS_DELETED;

		}	
		
		CLog::Log(LOG_LEVEL_WARNING,"Delete Task OK\n");
		nRet = 0;
	}
	
	return nRet;
}

//暂停任务，同时将相关的工作项暂停
INT CTaskManager::PauseTask(CTask *ptask){

	CWorkItem *pwi = NULL;
	INT size = 0;
	int i = 0;
	INT nRet = 0;
	if (!ptask){

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task error\n");
		nRet = -1;
	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task OK\n");
		ptask->m_task_status = CT_STATUS_PAUSED;

		std::vector<CWorkItem *> wilist = ptask->m_workitem_list;
		size = wilist.size();
		for(i =  0; i < size ;i ++ ){
			
			pwi = wilist[i];
			
			if ((pwi->m_wi_status == WI_STATUS_RUNNING ) || 
				(pwi->m_wi_status == WI_STATUS_WAITING)){
				pwi->m_wi_status = WI_STATUS_PAUSED;
			}
		}
		nRet = 0;
	}
	return nRet;

}

CWorkItem *CTaskManager::GetWorkItemByGuid(CTask *pTask,std::string str_guid){

	int size = 0;
	int i= 0;
	std::vector<CWorkItem *> wilist = pTask->m_workitem_list;
	CWorkItem *pwi = NULL;

	size = wilist.size();
	for(i = 0 ; i < size ;i ++ ){

		pwi = wilist[i];
		if (pwi->m_string_wi_guid.compare(str_guid) == 0){
			break;
		}
	}
	if (i == size){
		pwi = NULL;
	}

	return pwi;
}


INT CTaskManager::RecoveredTask(CTask *ptask,CWorkItem *pworkitem,std::string str_result){

	CWorkItem *pwi = NULL;
	int ret = 0;
	int wi_size = 0;
	int i = 0;
	std::vector<CWorkItem *> wilist = ptask->m_workitem_list;
	wi_size = wilist.size();

	ptask->m_string_result = str_result;
	ptask->m_crack_success = TRUE;  //找到密码
	ptask->m_finished_number = pTask->m_split_number;
	ptask->m_progress = 100.0;  //解密完成
	ptask->m_task_status = CT_STATUS_FINISHED; //任务结束


	for (i=0;i < wi_size;i++){
	
		pwi = wilist[i];
		RecoveredWorkItem(pwi);

	}
	
	return ret;
}
INT	CTaskManager::UnRecoveredTask(CTask *ptask){

	int ret = 0;

	ptask->m_crack_success = FALSE;  //未找到密码
	ptask->m_finished_number +=1;
	if (ptask->m_finished_number == ptask->m_split_number){

		ptask->m_progress = 100.0;
		ptask->m_task_status = CT_STATUS_FINISHED;

	}else {
	
		ptask->m_process = ((ptask->m_finished_number/ptask->m_split_number) * 100.0);

	}
		
	return ret;

}

INT CTaskManager::FailedTask(CTask *ptask){

	int ret = 0;
	ptask->m_crack_success = FALSE;  //未找到密码
//	ptask->m_task_status = CT_STATUS_FAILURE; //解密失败
	
	ptask->m_finished_number+=1;  //

	if (ptask->m_finished_number == ptask->m_split_number){

		ptask->m_progress = 100.0;
		ptask->m_task_status = CT_STATUS_FAILURE;
	}else {
		
		ptask->m_process = ((ptask->m_finished_number/ptask->m_split_number) * 100.0);
		
	}
			
	return ret;

}


//获得准备运行的工作项
CWorkItem *CTaskManager::GetReadyWI(CTask *pTask){

	CWorkItem *pwi = NULL;
	int ret = 0;
	int wi_size = 0;
	int i = 0;
	std::vector<CWorkItem *> wilist = ptask->m_workitem_list;
	wi_size = wilist.size();

	for (i=0;i < wi_size;i++){
	
		pwi = wilist[i];
		if (pwi->m_wi_status == WI_STATUS_WAITING){
				
				break;
		}
	}

	if (i == wi_size){

		pwi = NULL;
	}
	
	return pwi;

}