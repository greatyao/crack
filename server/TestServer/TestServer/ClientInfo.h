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

	char m_osinfo[64];	//����ϵͳ��Ϣ
	char m_ip[20];		//IP��ַ��Ϣ

	char m_type;		//�ͻ�������,control , compute
	
	char m_hostname[64];//��������
	char m_guid[40]; //�ڵ�guid
	
	UINT_PTR m_clientsock;
	int m_port;

	time_t m_keeplivetime;		//���һ������ʱ��
	
	time_t m_logintime;		//��¼ʱ��

};
