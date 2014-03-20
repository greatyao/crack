#include "StdAfx.h"
#include "Task.h"

CTask::CTask(void)
{

	m_string_ct_guid = new_guid();
	m_split_number = 0;
	m_finished_number = 0;
	m_crack_success = false;

	m_string_result = "";
	m_progress = 0.0 ;
	m_task_status  = CT_STATUS_READY;
	priority = TASK_PRIORITY_NORMAL;

}

CTask::~CTask(void)
{

}


//ccoretask
/***************************************************************
构造函数，初始化一些基本信息
***************************************************************/
/*
CTask::ccoretask()
{
	m_string_ct_guid = new_guid();
	m_split_number = 0;
	m_finished_number = 0;
	m_crack_success = false;

	m_string_result = "";
	m_progress = 0.0 ;

}
/***************************************************************
析构函数，清理资源信息
***************************************************************/
/*
CTask::~ccoretask()
{
	for(unsigned i=0; i<m_workitem_list.size(); i++)
	{
		cworkitem  *p = m_workitem_list[i];
		delete p;
	}
	m_workitem_list.clear();
}
/***************************************************************
直接返回guid字符串
***************************************************************/	
/*
string CTask::init(void)
{
	return m_string_ct_guid;
}

//coretask 有关操作都在这里
/***************************************************************
获取进度信息
***************************************************************/
/*
float CTask::get_progress(void)
{
	return m_progress;
}

/***************************************************************
插入一个workitem
***************************************************************/
/*
void CTask::insert_work_item(cworkitem *pworkitem)
{
	m_workitem_list.push_back(pworkitem);
	m_split_number++;
}


/***************************************************************
获取一个等待运算的workitem
***************************************************************/
/*
cworkitem *CTask::get_work_item(void)
{
	cworkitem *pwi = 0;
	if(m_progress>=100.0)
		return pwi;

	for(unsigned i=0; i<m_workitem_list.size(); i++)
	{
		cworkitem  *p = m_workitem_list[i];
		if( p->m_wi_status==WI_STATUS_WAITING )
		{
			pwi = p;
			p->update_status_to_running();
			break;
		}
	}

	return pwi;
}
/***************************************************************
更新一个workitem
***************************************************************/
/*
void CTask::update_work_item(string &sguid)
{	
	for(unsigned i=0; i<m_workitem_list.size(); i++)
	{
		cworkitem  *p = m_workitem_list[i];
		if( p->m_string_wi_guid==sguid )
		{
			m_finished_number++;
			p->update_status_to_finished();
			m_progress = (float)((float)m_finished_number/(float)m_split_number);
			break;
		}
	}

}


