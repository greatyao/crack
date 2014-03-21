#pragma once

#include "Task.h"

class CTaskSchedule
{
public:

	virtual CTask * Schedule(std::vector<CTask *> tasklist){
		
		return NULL;
	};
};
