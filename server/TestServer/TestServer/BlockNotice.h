#pragma once

#define STATUS_NOTICE_FINISH 1  //���һ��block ������ɣ��������ü�������
#define STATUS_NOTICE_RESET 2   //��Լ���ڵ���ߣ����������е�block ��Ҫ����״̬ΪReady 
#define STATUS_NOTICE_STOP	4   //���ֹͣ����֪ͨ����ڵ��block��ֹ����

#define STATUS_NOTICE_RUN 8 //���������е�block״̬

class CBlockNotice
{
public:
	CBlockNotice(void);
	~CBlockNotice(void);

public:

	char m_guid[40];
	char m_status;

};
