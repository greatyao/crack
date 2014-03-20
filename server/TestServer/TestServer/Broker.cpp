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
 上传任务，并切割保存，然后等待计算节点来获取并计算
*******************************************************************************************************************************/
/*
string CBroker::update_task(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon)
{
	//参数校验
	if( (min_len<1)||(max_len<min_len)||(string_chars_set.length()<1)||(split<1)||(jhon==0) )
	{
		CLog::Log(LOG_LEVEL_ERROR,"%s 参数错误，请检查参数\n",__FUNCTION__);
		return "";
	}

	lock();
	//切割任务
	CSplitChars sc;
	sc.Init(string_chars_set,min_len,max_len,split);

	std::string s_start;
	std::string s_end;
	std::string s_guid;
	
	ccoretask *pcoretask = new ccoretask();
	s_guid = pcoretask->init();

	//保存任务
	m_coretask_list.push_back(pcoretask);

	//分配一个或多个workitem
	for(unsigned i=1; i<=split; i++)
	{
		sc.Split(i,s_start,s_end);
		//循环插入workitem		
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


//添加获取guid 的任务状态
unsigned  CBroker::broker_get_coretask(string &string_guid){

	lock();
	unsigned ret = CT_STATUS_MAX;
	ccoretask *pct = 0;
	for (unsigned i = 0 ;i < m_coretask_list.size(); i++){

		pct = m_coretask_list[i];

		//查找guid相同的任务
		if (pct->m_string_ct_guid.compare(string_guid) == 0){
					
			//判断破解是否结束
			if (pct->m_progress >= 100.0){
				
				//判断破解是否成功
				if (pct->m_crack_success){

						ret = CT_STATUS_FINISHED;
						break;

				}else{
						//破解成功，但未产生结果
						ret = CT_STATUS_FAILURE;
						break;
				}

			}else{
				
				//破解正在运行中
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

		//查找guid相同的任务
		if (pct->m_string_ct_guid.compare(string_guid) == 0){
					

			//停止该任务
		//	pct->m_task_status = CT_STATUS_STOP;

			//判断破解是否结束
			if (pct->m_progress >= 100.0){
				
				//判断破解是否成功
				if (pct->m_crack_success){

						ret = CT_STATUS_FINISHED;
						break;

				}else{
						//破解成功，但未产生结果
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

		//查找guid相同的任务
		if (pct->m_string_ct_guid.compare(string_guid) == 0){
					
			//判断破解是否结束
			if (pct->m_progress >= 100.0){
				
				//判断破解是否成功
				if (pct->m_crack_success){

						pct->m_task_status = CT_STATUS_FINISHED;
						ret = CT_STATUS_FINISHED;
						break;

				}else{
						//破解成功，但未产生结果
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

