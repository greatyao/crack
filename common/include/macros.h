
#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

// BYTE���Ҳ��256
enum
{
	//// ���ƶ˷���������
	COMMAND_ACTIVED = 0x00,			// ����˿��Լ��ʼ����
	COMMAND_REMOVE,					// �����˳�
	COMMAND_REPLAY_HEARTBEAT,		// �ظ�������
	//
	//
	//// ����˷����ı�ʶ
	TOKEN_AUTH = 100,				// Ҫ����֤
	TOKEN_HEARTBEAT,				// ������
	TOKEN_LOGIN,					// ���߰�
	//
	////��ȡ�ͻ���
	////����
	CMD_KICK_OFF_CLIENT,
	//
	////��ȡһ������ķֽ��� WORKITEM
	CMD_GET_A_WORKITEM,
	//���ͽ��ܲ����õĽ��

	//�ϴ������ʱ�򣬷������˷�����������GUID��
	CMD_RET_TASK_GUID,

	//����GUID��ȡ���ܽ����Ϣ
	CMD_GET_A_TASK_RESULT,

	//�µ������
	//����ͻ���->������
	CMD_TASK_UPLOAD,	//�ϴ�����
	CMD_TASK_START,		//��ʼ����
	CMD_TASK_STOP,		//ֹͣ����
	CMD_TASK_DELETE,	//ɾ������
	CMD_TASK_PAUSE,		//��ͣ����
	CMD_TASK_RESULT,	//ȡ�ý�����Ϣ������״̬��

	CMD_REFRESH_STATUS,	//ȡ������Ľ��Ⱥ�״̬����Ϣ
	CMD_GET_CLIENT_LIST,//�������߼������Ϣ���б�
	
	CMD_WORKITEM_RECOVERED,		//�ҵ�����
	CMD_WORKITEM_UNRECOVERED,	//һ����������������Ϣ
	CMD_WORKITEM_GET,			//ȡ��workitem��������Ϣ
	CMD_WORKITEM_FAILED,		//һ���������ʧ�ܣ����ϣ�

	CMD_TEST_CMD,
	CMD_MAX_VALUE
};


#define	MAX_WRITE_RETRY			15 // ����д���ļ�����
#define	MAX_SEND_BUFFER			1024 * 8 // ��������ݳ���
#define MAX_RECV_BUFFER			1024 * 8 // ���������ݳ���

#endif // !defined(AFX_MACROS_H_INCLUDED)