#include "ClientInfo.h"
#include "macros.h"
#include "CLog.h"
#include <string.h>

CClientInfo::CClientInfo(void)
{
	m_clientsock = -1;
	memset(m_osinfo,0, sizeof(m_osinfo));
	memset(m_ip,0,20);
	memset(m_hostname,0,sizeof(m_hostname));
	memset(m_guid,0,40);
	m_port = 0;
	m_type = -1;

}

CClientInfo::~CClientInfo(void)
{
	if(m_clientsock > 0)
		closesocket(m_clientsock);
}

int CClientInfo::Init(const void* data, const char* ip, int port, unsigned int sock)
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

	if(this->m_type == COMPUTE_TYPE_CLIENT)
	{
		this->SetCPUGPU(pC->m_cputhreads, pC->m_gputhreads);
	}

	return 0;
}

