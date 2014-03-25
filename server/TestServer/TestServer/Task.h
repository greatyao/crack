#pragma once


#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <string>
#include <vector>
#include <iostream>
#include "guidgenerator.h"
#include "WorkItem.h"


/*******************************************************************
//coretask״̬����ʱδʹ�ã�
*******************************************************************/
typedef enum TASK_STATUS{

	//���������״̬
	CT_STATUS_READY = 1,   //����ľ���״̬
	CT_STATUS_FINISHED,		//�������״̬	
	CT_STATUS_FAILURE,		//�������ʧ��״̬
	CT_STATUS_RUNNING,		//�������ڽ���״̬
	CT_STATUS_PAUSED,		//������ͣ����״̬


	CT_STATUS_DELETED,		//����������Ϊɾ��״̬
	CT_STATUS_MAX

};


typedef enum TASK_PRIORITY{

	//��������ȼ�
	TASK_PRIORITY_NORMAL = 1,
	TASK_PRIORITY_IMPORTANT,
	

};


class CTask
{

public:
	std::string m_string_ct_guid;//coretask guid

	unsigned m_split_number;	//�и�������
	unsigned m_finished_number;	//���������

	bool m_crack_success;		//�ƽ��Ƿ�ɹ�	
	std::string m_string_result;//�ɹ��ƽ⵽������
	float m_progress;			//��ɶ� 0.0~100.0;

	unsigned char m_task_status;			//���������״̬,��ӦReady| Running | finished |
	unsigned char priority;		//�������ȼ�

	std::string m_algtype;		//�㷨����
	std::string m_hashinfo;		//Hash��Ϣ
	std::string m_ctlguid;		//���ƽڵ�guid

	
	std::vector <CWorkItem *> m_workitem_list;//workitem����
	
	/***************************************************************
	���캯������ʼ��һЩ������Ϣ
	***************************************************************/
//	ccoretask();
	/***************************************************************
	ֱ�ӷ���guid�ַ���
	***************************************************************/	
//	string init(void);	
	/***************************************************************
	����������������Դ��Ϣ
	***************************************************************/
//	~ccoretask();

	//coretask �йز�����������
	/***************************************************************
	��ȡ������Ϣ
	***************************************************************/
//	float get_progress(void);
	/***************************************************************
	����һ��workitem
	***************************************************************/
//	void insert_work_item(cworkitem *workitem);
	/***************************************************************
	��ȡһ���ȴ������workitem
	***************************************************************/
//	cworkitem *get_work_item(void);
	/***************************************************************
	����һ��workitem
	***************************************************************/
//	void update_work_item(string &sguid);

public:
	CTask(void);
	~CTask(void);
};
