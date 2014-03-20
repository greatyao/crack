#pragma once

#include "Task.h"

class CTaskManager: public CWorkItemManager
{

public:
	CTaskManager(void);
	~CTaskManager(void);

	//ÈÎÎñ²Ù×÷
	CTask* CreateTask();
	INT StartTask(CTask *ptask);
	INT StopTask(CTask *ptask);
	INT DeleteTask(CTask *ptask);
	INT PauseTask(CTask *ptask);
	
	INT RecoveredTask(CTask *ptask,std::string str_result);
	INT	UnRecoveredTask(CTask *ptask);
	INT FailedTask(CTask *ptask);
	CWorkItem *GetWorkItemByGuid(CTask *pTask,std::string str_guid);

	
	CWorkItem *GetReadyWI(CTask *pTask);

};
