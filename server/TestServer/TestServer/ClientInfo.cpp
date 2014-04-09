#include "ClientInfo.h"

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
}
