
#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

struct control_header
{
	unsigned char magic[5];			//G&CPU
	unsigned char cmd;				//������
	short response;					//��Ӧ״̬
	
	unsigned int dataLen;			//ԭʼ���ݳ���
	unsigned int compressLen;		//ѹ�����ݺ󳤶�
	unsigned int seq;				//��ţ�������ڶ�����ݰ�������Ŵ�1��ʼ���������Ϊ0��
									//�ж϶���������ݰ�������dataLen�Ƿ�Ϊ-1��
};

#define INITIALIZE_HEADER(cmd, resp, len, len2, seq)  {{'G', '&', 'C', 'P', 'U'}, cmd, resp, len, len2, seq}
#define INITIALIZE_EMPTY_HEADER(cmd)  {{'G', '&', 'C', 'P', 'U'}, cmd, 0, 0, 0, 0}

// BYTE���Ҳ��256
enum
{
	//// ���ƶ˷���������
	COMMAND_ACTIVED = 0x00,			// ����˿��Լ��ʼ����
	COMMAND_REMOVE,					// �����˳�
	COMMAND_REPLAY_HEARTBEAT,		// �ظ�������
	COMMAND_COMP_HEARTBEAT,			// �ظ�������
	
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

//����˷�������ڵ��֪�����״̬��������Ŀ�Ͳ���Ҫ�ٽ��н��ܵ�block
struct keeplive_compclient
{
	unsigned int tasks;		//����������
	unsigned int blocks;	//������Ҫ���н��ܵ�workitem��Ŀ
	typedef char block_guid[40];
	block_guid guids[0];
};

typedef enum Hash_STATUS_TYPE 
{	
	
	HASH_STATUS_READY,			//�������
	HASH_STATUS_FINISH,			//�������
	HASH_STATUS_RUNNING,		//����������
	
	HASH_STATUS_NO_PASS,		//���������δ�ҵ�����
	HASH_STATUS_FAILURE,		//���ܹ����з��ִ���

};

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
	WI_STATUS_LOCK,			//�������Ѿ�������ڵ��������ڱ�����ڵ㴦���� , ������ڵ�ռ��
	WI_STATUS_UNLOCK,		//���㵥Ԫ֪ͨ�����unlock����Դ����������ΪReady���Թ���������ڵ�ʹ��
	WI_STATUS_RUNNING,		//�������Ѿ�������ڵ��������ڱ�����ڵ㴦���� , ������ڵ�ռ��
	WI_STATUS_CRACKED,		//��ɽ���,�����н��ܽ��
	WI_STATUS_NO_PWD,		//���ܽ�������δ�ҵ�����
	WI_STATUS_NOT_NEED,		//������Ҫ����
	WI_STATUS_MAX
};



//�ͻ�������
typedef enum CLIENT_TYPE {
	
	COMPUTE_TYPE_CLIENT,
	CONTROL_TYPE_CLIENT,
	SUPER_CONTROL_TYPE_CLIENT,

};


typedef struct login_info {
	char m_osinfo[64];		//����ϵͳ��Ϣ
	char m_hostinfo[64];	//������
	char m_ip[16];			//IP��ַ��Ϣ
	unsigned short m_port;	//�˿�
	char m_type;			//�ͻ�������,control , compute
	int m_gputhreads;		//GPU��Ŀ
	int m_cputhreads;		//CPU��Ŀ
	
	unsigned int m_clientsock;
}client_login_req;

#define	MAX_WRITE_RETRY			15 // ����д���ļ�����
#define	MAX_SEND_BUFFER			1024 * 8 // ��������ݳ���
#define MAX_RECV_BUFFER			1024 * 8 // ���������ݳ���

#endif // !defined(AFX_MACROS_H_INCLUDED)