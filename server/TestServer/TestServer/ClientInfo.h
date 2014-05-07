#pragma once

#include <time.h>
#include <BaseTsd.h>
#include "CrackBroker.h"

struct login_info;
typedef std::map<string, CCrackTask *, MapLessCompare> CT_MAP2;

class CClientInfo
{
public:
	CClientInfo(void);
	~CClientInfo(void);

	int Init(const void* data, const char* ip, int port, unsigned int sock);

	virtual void SetCPUGPU(int cpu, int gpu){}

	char* GetIP() {return m_ip;}
	int GetPort() {return m_port;}
	SOCKET GetSocket() {return m_clientsock;}
	char* GetOwner() {return m_guid;}
	bool SuperUser() {return m_type == SUPER_CONTROL_TYPE_CLIENT;}

	bool OwnTask(const char* guid);
	void InsetTask(const char* guid, CCrackTask* task);
	void EraseTask(const char* guid, CCrackTask* task);

protected:
	friend class CCrackBroker;

	CT_MAP2 m_mytasks;

	char m_osinfo[64];	//操作系统信息
	char m_ip[20];		//IP地址信息

	char m_type;		//客户端类型,control , compute
	
	char m_hostname[64];//主机名称
	char m_guid[40]; //节点guid
	
	UINT_PTR m_clientsock;
	int m_port;

	time_t m_keeplivetime;		//最近一次心跳时间
	
	time_t m_logintime;		//登录时间

};
