#include "ClientInfo.h"
#include "macros.h"
#include "CLog.h"
#include "conn.h"
#include <string.h>

CClientInfo::CClientInfo(void)
{
	m_clientsock = 0;
	memset(m_osinfo,0, sizeof(m_osinfo));
	memset(m_ip,0,20);
	memset(m_hostname,0,sizeof(m_hostname));
	memset(m_guid,0,40);
	m_port = 0;
	m_type = -1;

}

CClientInfo::~CClientInfo(void)
{
	if(m_clientsock != 0)
		//closesocket(m_clientsock);
		conn_close((conn)m_clientsock);
}

bool CClientInfo::OwnTask(const char* guid)
{
	if(m_type == SUPER_CONTROL_TYPE_CLIENT) return true;
	return m_mytasks.find(guid) != m_mytasks.end();
}

void CClientInfo::InsetTask(const char* guid, CCrackTask* task)
{
	m_mytasks[guid] = task;
}

void CClientInfo::EraseTask(const char* guid, CCrackTask* task)
{
	if(guid)
	{
		CT_MAP2::iterator it = m_mytasks.find(guid);
		if(it != m_mytasks.end())
			m_mytasks.erase(it);
	}
	else if(task)
	{
		CT_MAP2::iterator it;
		for(it = m_mytasks.begin(); it != m_mytasks.end(); it++)
		{
			if(it->second == task)
			{
				m_mytasks.erase(it);
				break;
			}
		}
	}
}

int CClientInfo::Init(const void* data, const char* ip, int port, UINT_PTR sock)
{
	client_login_req *pC = (client_login_req *)data;
	CLog::Log(LOG_LEVEL_NOMAL, "[%s:%d] os=%s host=%s\n", ip, port, pC->m_osinfo, pC->m_hostinfo);

	this->m_clientsock = sock;
	this->m_type = pC->m_type;
	memcpy(this->m_ip, ip,16);
	this->m_port = port;
	memcpy(this->m_osinfo, pC->m_osinfo,sizeof(this->m_osinfo));
	memcpy(this->m_hostname,pC->m_hostinfo, sizeof(this->m_hostname));
	this->m_logintime = time(NULL);
	//这里guid是client的唯一标志,
	memcpy(this->m_guid, pC->m_hostinfo, sizeof(this->m_guid));

	if(this->m_type == COMPUTE_TYPE_CLIENT)
	{
		this->SetCPUGPU(pC->m_cputhreads, pC->m_gputhreads);
	}
	else if(this->m_type == SUPER_CONTROL_TYPE_CLIENT)
	{
		strcpy(this->m_hostname, "SuperAdmin");
	}

	return 0;
}

