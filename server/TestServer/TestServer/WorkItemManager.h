#pragma once

#include "WorkItem.h"

class CWorkItemManager
{
public:
	CWorkItemManager(void);
	~CWorkItemManager(void);

	CWorkItem* CreateWorkItem();
	INT StartWorkitem(CWorkItem *pworkitem);
	INT StopWorkitem(CWorkItem *pworkitem);
	INT DeleteWorkitem(CWorkItem *pworkitem);
	INT PauseWorkitem(CWorkItem *pworkitem);

	VOID RecoveredWorkItem(CWorkItem *pworkitem);
//	VOID UnRecoveredWorkItem(CWorkItem *pworkitem);
//	VOID FailedWorkItem(CWorkItem *pworkitem);

	INT UpdateStatusReady(CWorkItem *pworkitem);
	INT UpdateStatusRuning(CWorkItem *pworkitem);
	INT UpdateStatusFinished(CWorkItem *pworkitem);
	INT UpdateStatusNotNeed(CWorkItem *pworkitem);

	/*
	WI_STATUS_NO_PWD,				//���ܽ�������δ�ҵ�����
	WI_STATUS_FAILURE,		//������ִ��ʧ��
	*/

	INT UpdateStatusNoPWD(CWorkItem *pworkitem);
	INT UpdateStatusFailure(CWorkItem *pworkitem);

	
};
