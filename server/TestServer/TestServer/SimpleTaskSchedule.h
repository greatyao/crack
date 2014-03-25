#pragma once
#include "taskschedule.h"

class CSimpleTaskSchedule :
	public CTaskSchedule
{
public:

	CTask * Schedule(std::vector<CTask *> tasklist);
};
