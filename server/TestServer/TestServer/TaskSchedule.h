#pragma once

#include "Task.h"

class CTaskSchedule
{
public:
	CTaskSchedule(void);
	~CTaskSchedule(void);

	virtual CTask * Schedule(std::vector<CTask *> tasklist) = 0;
};
