#pragma once

#include <vector>
#include "algorithm_types.h"

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
	
	//New workitem define
/*	unsigned char m_algo;		//�����㷨
	unsigned char m_charset;	//�����ַ���
	unsigned char m_type;		//��������
	unsigned char m_special;	//�Ƿ����ļ����ܣ�pdf+office+rar+zip��
	char m_guid[40];			//����˵�workitem��GUID
	char m_john[sizeof(struct crack_hash)];		//ԭʼHash��ʽ��hashֵ+��
	unsigned short m_start;	//��ʼ����
	unsigned short m_end;		//��������
	//��������������
	unsigned short m_start2;	//55555-99999:start2=5,end2=9	000-55555:start2=0,end2=5
	unsigned short m_end2;
	char m_custom[0]; //�û��Զ�����ַ���
*/
	//Old workitem define
	
	std::string m_string_wi_guid;	//workitem��guid

	std::string m_string_wi_john;	//Ŀ���ƽ����Ϣ
	std::string m_string_wi_begin;	//�����ƽ�Ŀ�ʼ�ַ�
	std::string m_string_wi_end;	//�����ƽ�Ľ����ַ�
	std::string m_string_wi_chars_set;//�����ƽ���ַ���

	unsigned char m_wi_status;	//workitem��ǰ״̬
	std::string m_comp_guid; //����ļ���ڵ�guid

	
	void * m_ptask;	//ָ��workitem ���ڵ�����


public:
	CWorkItem(void);
	~CWorkItem(void);

	/***************************************************************
	��ʼ��workitem��Ϣ��������guid�ַ���
	***************************************************************/
	//std::string init(std::string sjohn,std::string sbegin,std::string send,std::string scset);
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
