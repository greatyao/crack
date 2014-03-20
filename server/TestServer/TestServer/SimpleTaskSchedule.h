#pragma once
#include "taskschedule.h"

class CSimpleTaskSchedule :
	public CTaskSchedule
{
public:
	CSimpleTaskSchedule(void);
	~CSimpleTaskSchedule(void);

	//CTask * Schedule(std::vector<CTask *> tasklist);
};
