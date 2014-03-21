#include "StdAfx.h"
#include "WorkItemManager.h"
#include "CLog.h"


CWorkItemManager::CWorkItemManager(void)
{
}

CWorkItemManager::~CWorkItemManager(void)
{
}

CWorkItem* CWorkItemManager::CreateWorkItem(){

	CWorkItem *pWI = new CWorkItem();

	if (!pWI){
	
		CLog::Log(LOG_LEVEL_WARNING,"Ceate WorkItem Error\n");

		pWI = NULL;
	}
	return pWI;

}


//执行工作项
INT CWorkItemManager::StartWorkitem(CWorkItem *pworkitem){

	return UpdateStatusRuning(pworkitem);
	
}

//停止工作项执行
INT CWorkItemManager::StopWorkitem(CWorkItem *pworkitem){

	return UpdateStatusFinished(pworkitem);
}

//删除工作项
INT CWorkItemManager::DeleteWorkitem(CWorkItem *pworkitem){

	INT nRet = 0;
	if (!pworkitem){

		CLog::Log(LOG_LEVEL_WARNING,"delete workitem error\n");
		nRet = -1;

	}else{
		delete pworkitem;
	}
	return nRet;

}

//暂停工作项执行
INT CWorkItemManager::PauseWorkitem(CWorkItem *pworkitem){

	return UpdateStatusReady(pworkitem);
	
}


//更新工作项状态接口

INT CWorkItemManager::UpdateStatusReady(CWorkItem *pworkitem){

		
	INT nRet = 0 ;
	if (!pworkitem){
		CLog::Log(LOG_LEVEL_WARNING,"WorkItem update ready error\n");
		nRet = -1;
	}else{
		pworkitem->m_wi_status = WI_STATUS_WAITING;
		nRet = 0;		
	}
	return nRet;
}


INT CWorkItemManager::UpdateStatusRuning(CWorkItem *pworkitem){


	INT nRet = 0 ;
	if (!pworkitem){
		CLog::Log(LOG_LEVEL_WARNING,"WorkItem update running error\n");
		nRet = -1;
	}else{
		pworkitem->m_wi_status = WI_STATUS_RUNNING;
		nRet = 0;		
	}
	return nRet;
}

//解密结束，且找到解密结果
INT CWorkItemManager::UpdateStatusFinished(CWorkItem *pworkitem){

	INT nRet = 0 ;
	if (!pworkitem){
		CLog::Log(LOG_LEVEL_WARNING,"WorkItem Update finished error\n");
		nRet = -1;
	}else{
		pworkitem->m_wi_status = WI_STATUS_FINISHED;
		nRet = 0;		
	}
	return nRet;

}

//解密结束，未找到解密结果
INT CWorkItemManager::UpdateStatusNoPWD(CWorkItem *pworkitem){

	INT nRet = 0 ;
	if (!pworkitem){
		CLog::Log(LOG_LEVEL_WARNING,"WorkItem Update No Pwd error\n");
		nRet = -1;
	}else{
		pworkitem->m_wi_status = WI_STATUS_NO_PWD;
		nRet = 0;		
	}
	return nRet;

}

//解密结束，解密失败
INT CWorkItemManager::UpdateStatusFailure(CWorkItem *pworkitem){

	INT nRet = 0 ;
	if (!pworkitem){
		CLog::Log(LOG_LEVEL_WARNING,"WorkItem Update Failure error\n");
		nRet = -1;
	}else{
		pworkitem->m_wi_status = WI_STATUS_FAILURE;
		nRet = 0;		
	}
	return nRet;


}


INT CWorkItemManager::UpdateStatusNotNeed(CWorkItem *pworkitem){


	INT nRet = 0 ;
	if (!pworkitem){
		CLog::Log(LOG_LEVEL_WARNING,"WorkItem Update not need error\n");
		nRet = -1;
	}else{
		pworkitem->m_wi_status = WI_STATUS_NOT_NEED;
		nRet = 0;		
	}
	return nRet;
}






VOID CWorkItemManager::RecoveredWorkItem(CWorkItem *pworkitem){

	if ((pworkitem->m_wi_status != WI_STATUS_FINISHED) && 
		(pworkitem->m_wi_status != WI_STATUS_NOT_NEED) &&
		(pworkitem->m_wi_status != WI_STATUS_DELETED)) {
	
			pworkitem->m_wi_status = WI_STATUS_NOT_NEED;
	}


}


/*
VOID CWorkItemManager::UnRecoveredWorkItem(CWorkItem *pworkitem){


	
}

VOID CWorkItemManager::FailedWorkItem(CWorkItem *pworkitem){


}
*/



