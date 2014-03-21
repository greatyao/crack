#include "StdAfx.h"
#include "SimpleTaskSchedule.h"



//һ���򵥵�ʵ��
CTask * CSimpleTaskSchedule::Schedule(std::vector<CTask*> tasklist){

	int task_count = tasklist.size();
	CTask *ptask = NULL;
	CTask *ptmp = NULL;
	int i = 0;


	//���Ȳ���
	//����δ���,
	//���ȼ�����ȵ��ȣ�
	//���ھ���(�ȴ�ִ��)������ִ��(���ܰ���δִ�еĹ�����)״̬�������ȵ���
	//����������״̬���߾���״̬

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