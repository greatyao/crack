#pragma once

#include <crtdefs.h>

class CClientInfo
{
public:
	CClientInfo(void);
	~CClientInfo(void);

public:
	
	char m_osinfo[16];	//����ϵͳ��Ϣ
	char m_ip[20];		//IP��ַ��Ϣ

	char m_type;		//�ͻ�������,control , compute
	
	char m_hostname[50];//��������
	unsigned char m_guid[40]; //�ڵ�guid
	
	unsigned int m_clientsock;
	int m_port;

	time_t m_keeplivetime;		//���һ������ʱ��
	
	time_t m_logintime;		//��¼ʱ��

};
