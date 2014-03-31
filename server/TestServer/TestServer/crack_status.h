
#ifndef __CRACK_STATUS_H_
#define __CRACK_STATUS_H_


typedef enum CRACK_TASK_STATUS {

	CT_STATUS_READY = 1,   //����ľ���״̬
	CT_STATUS_FINISHED,		//�������״̬	
	CT_STATUS_FAILURE,		//�������ʧ��״̬
	CT_STATUS_RUNNING,		//�������ڽ���״̬

//	CT_STATUS_STOPPED,		//����ֹͣ״̬  
	CT_STATUS_PAUSED,		//������ͣ����״̬
	CT_STATUS_DELETED,		//����������Ϊɾ��״̬
	CT_STATUS_MAX
	
};

typedef enum CRACK_TASK_PRIORITY{


	CT_PRIORITY_NORMAL = 1,
	CT_PRIORITY_IMPORTANT,

};

typedef enum CRACK_BLOCK_STATUS{

	WI_STATUS_WAITING = 1,	//����ù�����󣬹�����ĳ�ʼ״̬ 
	WI_STATUS_READY,		//���������󣬹������״̬ ,start task
	WI_STATUS_RUNNING,		//�������Ѿ�������ڵ��������ڱ�����ڵ㴦���� , ������ڵ�ռ��
	WI_STATUS_FINISHED,		//��ɽ���,�����н��ܽ��
	WI_STATUS_NO_PWD,		//���ܽ�������δ�ҵ�����
	WI_STATUS_FAILURE,		//������ִ��ʧ��

	WI_STATUS_NOT_NEED,		//������Ҫ����

//	WI_STATUS_STOPPED,		//����ֹͣ����صĹ�����Ӧ�ñ�ֹͣ
//	WI_STATUS_PAUSED,		//������ͣ����صĹ�����Ӧ�ñ���ͣ
//	WI_STATUS_DELETED,		//������������Ϊɾ��״̬
	
	WI_STATUS_MAX

};



//�ͻ�������
typedef enum CLIENT_TYPE {
	
	COMPUTE_TYPE_CLIENT,
	CONTROL_TYPE_CLIENT,
	SUPER_CONTROL_TYPE_CLIENT,

};

#endif



