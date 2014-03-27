
#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

struct control_header
{
	unsigned char magic[5];			//G&CPU
	unsigned char cmd;				//������
	short response;					//��Ӧ״̬
	unsigned int dataLen;			//ԭʼ���ݳ���
	unsigned int compressLen;		//ѹ�����ݺ󳤶�
};

#define INITIALIZE_HEADER(cmd, resp, len, len2)  {{'G', '&', 'C', 'P', 'U'}, cmd, resp, len, len2}
#define INITIALIZE_EMPTY_HEADER(cmd)  {{'G', '&', 'C', 'P', 'U'}, cmd, 0, 0, 0}

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
	
	//// (���㵥Ԫ�ͷ����)
	CMD_GET_A_WORKITEM,		//��ȡһ������ķֽ��� WORKITEM
	CMD_WORKITEM_STATUS,	//���㵥Ԫ�ϱ�����״̬
	CMD_WORKITEM_RESULT,	//���㵥Ԫ�ϱ����ܽ��
	

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
	
	//�ļ�����
	CMD_DOWNLOAD_FILE,			//�����ļ�
	CMD_UPLOAD_FILE,			//�ϴ��ļ�
	CMD_START_DOWNLOAD,			//��ʼ�����ļ�
	CMD_START_UPLOAD,			//��ʼ�ϴ��ļ�
	CMD_END_DOWNLOAD,			//���������ļ�
	CMD_END_UPLOAD,				//�����ϴ��ļ�

	CMD_TEST_CMD,
	CMD_MAX_VALUE
};

struct file_info
{
	void* f;			//�ļ�fd
	unsigned int len;	//�ļ�����
	unsigned int offset;//�ļ��α�
};



#define	MAX_WRITE_RETRY			15 // ����д���ļ�����
#define	MAX_SEND_BUFFER			1024 * 8 // ��������ݳ���
#define MAX_RECV_BUFFER			1024 * 8 // ���������ݳ���

#endif // !defined(AFX_MACROS_H_INCLUDED)