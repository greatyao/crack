#include "StdAfx.h"
#include "Broker.h"

#include "guidgenerator.h"

#include "SplitChars.h"

#include "CLog.h"


CBroker::CBroker(void)
{
}

CBroker::~CBroker(void)
{
}


/*
//cbroker
void CBroker::lock(void)
{
	EnterCriticalSection(&m_critical_section_broker);
}
void CBroker::unlock(void)
{
	LeaveCriticalSection(&m_critical_section_broker);
}
CBroker::cbroker()
{
	InitializeCriticalSection(&m_critical_section_broker);
}
CBroker::~cbroker()
{
	DeleteCriticalSection(&m_critical_section_broker);
}
*/

/*******************************************************************************************************************************
 �ϴ����񣬲��и�棬Ȼ��ȴ�����ڵ�����ȡ������
*******************************************************************************************************************************/
/*
string CBroker::update_task(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon)
{
	//����У��
	if( (min_len<1)||(max_len<min_len)||(string_chars_set.length()<1)||(split<1)||(jhon==0) )
	{
		CLog::Log(LOG_LEVEL_ERROR,"%s ���������������\n",__FUNCTION__);
		return "";
	}

	lock();
	//�и�����
	CSplitChars sc;
	sc.Init(string_chars_set,min_len,max_len,split);

	std::string s_start;
	std::string s_end;
	std::string s_guid;
	
	ccoretask *pcoretask = new ccoretask();
	s_guid = pcoretask->init();

	//��������
	m_coretask_list.push_back(pcoretask);

	//����һ������workitem
	for(unsigned i=1; i<=split; i++)
	{
		sc.Split(i,s_start,s_end);
		//ѭ������workitem		
		cworkitem *pwi = new cworkitem();
		pwi->init(jhon,s_start,s_end,string_chars_set);
		pcoretask->insert_work_item(pwi);
	}
	unlock();
	return s_guid;
}
cworkitem *CBroker::broker_get_work_item(void)
{
	lock();
	cworkitem *pwi = 0;

	for(unsigned i=0; i<m_coretask_list.size(); i++)
	{
		ccoretask * p = m_coretask_list[i];
		pwi = p->get_work_item();
		if(pwi) break;
	}

	unlock();
	return pwi;
}


//��ӻ�ȡguid ������״̬
unsigned  CBroker::broker_get_coretask(string &string_guid){

	lock();
	unsigned ret = CT_STATUS_MAX;
	ccoretask *pct = 0;
	for (unsigned i = 0 ;i < m_coretask_list.size(); i++){

		pct = m_coretask_list[i];

		//����guid��ͬ������
		if (pct->m_string_ct_guid.compare(string_guid) == 0){
					
			//�ж��ƽ��Ƿ����
			if (pct->m_progress >= 100.0){
				
				//�ж��ƽ��Ƿ�ɹ�
				if (pct->m_crack_success){

						ret = CT_STATUS_FINISHED;
						break;

				}else{
						//�ƽ�ɹ�����δ�������
						ret = CT_STATUS_FAILURE;
						break;
				}

			}else{
				
				//�ƽ�����������
				ret = CT_STATUS_RUNNING;
				break;

			}

		}

	}
	unlock();
	return ret;
}



//add 2014-03-10 

//start task ,split the task into work item




//stop task 


unsigned  CBroker::broker_stop_coretask(string &string_guid){

	lock();
	unsigned ret = CT_STATUS_MAX;
	ccoretask *pct = 0;
	for (unsigned i = 0 ;i < m_coretask_list.size(); i++){

		pct = m_coretask_list[i];

		//����guid��ͬ������
		if (pct->m_string_ct_guid.compare(string_guid) == 0){
					

			//ֹͣ������
		//	pct->m_task_status = CT_STATUS_STOP;

			//�ж��ƽ��Ƿ����
			if (pct->m_progress >= 100.0){
				
				//�ж��ƽ��Ƿ�ɹ�
				if (pct->m_crack_success){

						ret = CT_STATUS_FINISHED;
						break;

				}else{
						//�ƽ�ɹ�����δ�������
						ret = CT_STATUS_FAILURE;
						break;
				}

			}else{
				
				pct->m_task_status = CT_STATUS_READY;
				ret = CT_STATUS_READY;
				break;
			}

		}

	}
	unlock();
	return ret;
}





//pause task 
unsigned  CBroker::broker_pause_coretask(string &string_guid){

	lock();
	unsigned ret = CT_STATUS_MAX;
	ccoretask *pct = 0;
	for (unsigned i = 0 ;i < m_coretask_list.size(); i++){

		pct = m_coretask_list[i];

		//����guid��ͬ������
		if (pct->m_string_ct_guid.compare(string_guid) == 0){
					
			//�ж��ƽ��Ƿ����
			if (pct->m_progress >= 100.0){
				
				//�ж��ƽ��Ƿ�ɹ�
				if (pct->m_crack_success){

						pct->m_task_status = CT_STATUS_FINISHED;
						ret = CT_STATUS_FINISHED;
						break;

				}else{
						//�ƽ�ɹ�����δ�������
						pct->m_task_status = CT_STATUS_FAILURE;
						ret = CT_STATUS_FAILURE;
						break;
				}

			}else{
				
				pct->m_task_status = CT_STATUS_PAUSED;
				ret = CT_STATUS_PAUSED;
				break;

			}

		}

	}
	unlock();
	return ret;
}
*/

