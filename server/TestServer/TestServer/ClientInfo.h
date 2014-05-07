#pragma once

#include <time.h>

struct login_info;

class CClientInfo
{
public:
	CClientInfo(void);
	~CClientInfo(void);

	int Init(const void* data, const char* ip, int port, unsigned int sock);

	virtual void SetCPUGPU(int cpu, int gpu){}

public:
	
	char m_osinfo[64];	//����ϵͳ��Ϣ
	char m_ip[20];		//IP��ַ��Ϣ

	char m_type;		//�ͻ�������,control , compute
	
	char m_hostname[64];//��������
	unsigned char m_guid[40]; //�ڵ�guid
	
	unsigned int m_clientsock;
	int m_port;

	time_t m_keeplivetime;		//���һ������ʱ��
	
	time_t m_logintime;		//��¼ʱ��

};
