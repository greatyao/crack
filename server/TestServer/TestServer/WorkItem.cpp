#include "StdAfx.h"
#include "WorkItem.h"

CWorkItem::CWorkItem(void)
{
}

CWorkItem::~CWorkItem(void)
{
}


//cworkitem
/***************************************************************
��ʼ��workitem��Ϣ��������guid�ַ���
***************************************************************/
std::string CWorkItem::init(string sjohn,string sbegin,string send,string scset)
{
	m_string_wi_guid	= new_guid();	
	m_wi_status			= WI_STATUS_WAITING; 

	m_string_wi_john	=sjohn;	
	m_string_wi_begin	=sbegin;	
	m_string_wi_end		=send;	
	m_string_wi_chars_set=scset;

	m_comp_guid = "";
	m_ptask = NULL;

	return m_string_wi_guid;
};
