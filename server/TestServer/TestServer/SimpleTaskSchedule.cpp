#include "StdAfx.h"
#include "SimpleTaskSchedule.h"



//一个简单的实现
CTask * CSimpleTaskSchedule::Schedule(std::vector<CTask*> tasklist){

	int task_count = tasklist.size();
	CTask *ptask = NULL;
	CTask *ptmp = NULL;
	int i = 0;


	//调度策略
	//任务未完成,
	//优先级大的先调度，
	//处于就绪(等待执行)和正在执行(可能包含未执行的工作项)状态的任务先调度
	//任务处于运行状态或者就绪状态

	for (i = 1; i < task_count ;i ++ ){

		ptask = tasklist[i];

		if ((ptask->m_progress < 100.0) && (ptask->m_task_status == CT_STATUS_RUNNING)) {

			if (ptmp){

				if(ptask->priority > ptmp->priority){

					ptmp = ptask;
				}
			}else{

				ptmp = ptask;

			}

		}

	}

	if (!ptmp){

		ptask = NULL;

		for (i = 1; i < task_count ;i ++ ){

			ptask = tasklist[i];

			if ((ptask->m_progress < 100.0) && (ptask->m_task_status == CT_STATUS_READY)) {

				if (ptmp){

					if(ptask->priority > ptmp->priority){

						ptmp = ptask;
					}
				}else{

					ptmp = ptask;

				}

			}

		}
	}

	return ptmp;
}