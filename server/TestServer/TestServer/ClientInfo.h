#pragma once

#include <crtdefs.h>

class CClientInfo
{
public:
	CClientInfo(void);
	~CClientInfo(void);

public:
	
	char m_osinfo[16];	//操作系统信息
	char m_ip[20];		//IP地址信息

	char m_type;		//客户端类型,control , compute
	
	char m_hostname[50];//主机名称
	unsigned char m_guid[40]; //节点guid
	
	unsigned int m_clientsock;
	int m_port;

	time_t m_keeplivetime;		//最近一次心跳时间
	
	time_t m_logintime;		//登录时间

};
