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
���캯������ʼ��һЩ������Ϣ
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
����������������Դ��Ϣ
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
ֱ�ӷ���guid�ַ���
***************************************************************/	
/*
string CTask::init(void)
{
	return m_string_ct_guid;
}

//coretask �йز�����������
/***************************************************************
��ȡ������Ϣ
***************************************************************/
/*
float CTask::get_progress(void)
{
	return m_progress;
}

/***************************************************************
����һ��workitem
***************************************************************/
/*
void CTask::insert_work_item(cworkitem *pworkitem)
{
	m_workitem_list.push_back(pworkitem);
	m_split_number++;
}


/***************************************************************
��ȡһ���ȴ������workitem
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
����һ��workitem
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


