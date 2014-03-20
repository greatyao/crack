#pragma once

#include <vector>

/*******************************************************************
//workitem״̬���
*******************************************************************/
enum{
	WI_STATUS_WAITING = 1,	//����������Ժ󣬴��ڵȴ�����ڵ㴦��
	WI_STATUS_RUNNING,		//���ڱ�����ڵ㴦����
	WI_STATUS_FINISHED,		//��ɽ���,�����н��ܽ��
	WI_STATUS_NOT_NEED,		//������Ҫ����

	WI_STATUS_STOPPED,			//����ֹͣ����صĹ�����Ӧ�ñ�ֹͣ
	WI_STATUS_PAUSED,		//������ͣ����صĹ�����Ӧ�ñ���ͣ
	WI_STATUS_DELETED,		//������������Ϊɾ��״̬

	WI_STATUS_NO_PWD,				//���ܽ�������δ�ҵ�����
	WI_STATUS_FAILURE,		//������ִ��ʧ��
	WI_STATUS_MAX
};



class CWorkItem
{

public:

	std::string m_string_wi_guid;	//workitem��guid

	std::string m_string_wi_john;	//Ŀ���ƽ����Ϣ
	std::string m_string_wi_begin;	//�����ƽ�Ŀ�ʼ�ַ�
	std::string m_string_wi_end;	//�����ƽ�Ľ����ַ�
	std::string m_string_wi_chars_set;//�����ƽ���ַ���

	unsigned char m_wi_status;	//workitem��ǰ״̬
	std::string m_comp_guid; //����ļ���ڵ�guid
	
	LPVOID	* m_ptask;	//ָ��workitem ���ڵ�����


public:
	CWorkItem(void);
	~CWorkItem(void);

	/***************************************************************
	��ʼ��workitem��Ϣ��������guid�ַ���
	***************************************************************/
	//string init(string sjohn,string sbegin,string send,string scset);
	/***************************************************************
	����workitem״̬
	***************************************************************/
	/*
	void update_status_to_running(void);
	void update_status_to_finished(void);
	void update_status_to_not_need(void);
	void update_status_to_waiting(void);
	*/
};
