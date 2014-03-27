#include "StdAfx.h"
#include "WorkItem.h"
#include "GuidGenerator.h"


CWorkItem::CWorkItem(void)
{
}

/*
CWorkItem::CWorkItem(crack_block cb)
{
	m_algo = cb.algo;
	m_charset = cb.charset;
	m_type = cb.type;
	m_special = cb.special;
	memset(m_guid,0,40);
	memcpy(m_guid,cb.guid,40);

	memset(m_john,0,sizeof(struct crack_hash));
	memcpy(m_john,cb.john,sizeof(struct crack_hash));

	m_start = cb.start;
	m_end = cb.end;

	m_start2 = cb.start2;
	m_end2 = cb.end2;

}
*/

CWorkItem::~CWorkItem(void)
{
}


//cworkitem
/***************************************************************
初始化workitem信息，并返回guid字符串
***************************************************************/
/*
std::string CWorkItem::init(std::string sjohn,std::string sbegin,std::string send,std::string scset)
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

*/



