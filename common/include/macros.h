
#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

struct control_header
{
	unsigned char magic[5];			//G&CPU
	unsigned char cmd;				//命令字
	short response;					//回应状态
	
	unsigned int dataLen;			//原始数据长度
	unsigned int compressLen;		//压缩数据后长度
	unsigned int seq;				//序号（如果存在多个数据包，则序号从1开始，否则序号为0。
									//判断多个序列数据包结束看dataLen是否为-1）
};

#define INITIALIZE_HEADER(cmd, resp, len, len2, seq)  {{'G', '&', 'C', 'P', 'U'}, cmd, resp, len, len2, seq}
#define INITIALIZE_EMPTY_HEADER(cmd)  {{'G', '&', 'C', 'P', 'U'}, cmd, 0, 0, 0, 0}

// BYTE最大也就256
enum
{
	//// 控制端发出的命令
	COMMAND_ACTIVED = 0x00,			// 服务端可以激活开始工作
	COMMAND_REMOVE,					// 程序退出
	COMMAND_REPLAY_HEARTBEAT,		// 回复心跳包
	COMMAND_COMP_HEARTBEAT,			// 回复心跳包
	
	//
	//
	//// 服务端发出的标识
	TOKEN_AUTH = 100,				// 要求验证
	TOKEN_HEARTBEAT,				// 心跳包
	TOKEN_LOGIN,					// 上线包
	//
	////获取客户端
	////踢人
	CMD_KICK_OFF_CLIENT,
	//
	
	//// (计算单元和服务端)
	CMD_GET_A_WORKITEM,		//获取一条任务的分解项 WORKITEM
	CMD_WORKITEM_STATUS,	//计算单元上报解密状态
	CMD_WORKITEM_RESULT,	//计算单元上报解密结果
	

	//上传任务的时候，服务器端返回任务分配的GUID号
	CMD_RET_TASK_GUID,

	//根据GUID获取解密结果信息
	CMD_GET_A_TASK_RESULT,

	//新的命令定义
	//管理客户端->服务器
	CMD_TASK_UPLOAD,	//上传任务
	CMD_TASK_START,		//开始任务
	CMD_TASK_STOP,		//停止任务
	CMD_TASK_DELETE,	//删除任务
	CMD_TASK_PAUSE,		//暂停任务
	CMD_TASK_RESULT,	//取得解密信息（解密状态）

	CMD_REFRESH_STATUS,	//取得任务的进度和状态等信息
	CMD_GET_CLIENT_LIST,//返回在线计算机信息的列表
	
	CMD_WORKITEM_RECOVERED,		//找到密码
	CMD_WORKITEM_UNRECOVERED,	//一个运算结束，标记信息
	CMD_WORKITEM_GET,			//取得workitem的内容信息
	CMD_WORKITEM_FAILED,		//一个运算解密失败（故障）
	
	//文件传输
	CMD_DOWNLOAD_FILE,			//下载文件
	CMD_UPLOAD_FILE,			//上传文件
	CMD_START_DOWNLOAD,			//开始下载文件
	CMD_START_UPLOAD,			//开始上传文件
	CMD_END_DOWNLOAD,			//结束下载文件
	CMD_END_UPLOAD,				//结束上传文件

	CMD_TEST_CMD,
	CMD_MAX_VALUE
};

struct file_info
{
	void* f;			//文件fd
	unsigned int len;	//文件长度
	unsigned int offset;//文件游标
};

//服务端发给计算节点告知其就绪状态的任务数目和不需要再进行解密的block
struct keeplive_compclient
{
	unsigned int tasks;		//就绪任务数
	unsigned int blocks;	//不再需要进行解密的workitem数目
	typedef char block_guid[40];
	block_guid guids[0];
};

typedef enum Hash_STATUS_TYPE 
{	
	
	HASH_STATUS_READY,			//任务就绪
	HASH_STATUS_FINISH,			//任务完成
	HASH_STATUS_RUNNING,		//任务运行中
	
	HASH_STATUS_NO_PASS,		//任务结束，未找到密码
	HASH_STATUS_FAILURE,		//解密过程中发现错误

};

typedef enum CRACK_TASK_STATUS {

	CT_STATUS_READY = 1,   //任务的就绪状态
	CT_STATUS_FINISHED,		//任务完成状态	
	CT_STATUS_FAILURE,		//任务解密失败状态
	CT_STATUS_RUNNING,		//任务正在解密状态

//	CT_STATUS_STOPPED,		//任务停止状态  
	CT_STATUS_PAUSED,		//任务暂停解密状态
	CT_STATUS_DELETED,		//将任务设置为删除状态
	CT_STATUS_MAX
	
};

typedef enum CRACK_TASK_PRIORITY{


	CT_PRIORITY_NORMAL = 1,
	CT_PRIORITY_IMPORTANT,

};

typedef enum CRACK_BLOCK_STATUS{

	WI_STATUS_WAITING = 1,	//分配好工作项后，工作项的初始状态 
	WI_STATUS_READY,		//任务启动后，工作项的状态 ,start task
	WI_STATUS_LOCK,			//工作项已经被计算节点请求，正在被计算节点处理中 , 被计算节点占用
	WI_STATUS_UNLOCK,		//计算单元通知服务端unlock该资源，重新设置为Ready，以供其他计算节点使用
	WI_STATUS_RUNNING,		//工作项已经被计算节点请求，正在被计算节点处理中 , 被计算节点占用
	WI_STATUS_CRACKED,		//完成结束,包含有解密结果
	WI_STATUS_NO_PWD,		//解密结束但是未找到密码
	WI_STATUS_NOT_NEED,		//任务不需要处理
	WI_STATUS_MAX
};



//客户端类型
typedef enum CLIENT_TYPE {
	
	COMPUTE_TYPE_CLIENT,
	CONTROL_TYPE_CLIENT,
	SUPER_CONTROL_TYPE_CLIENT,

};


typedef struct login_info {
	char m_osinfo[64];		//操作系统信息
	char m_hostinfo[64];	//机器名
	char m_ip[16];			//IP地址信息
	unsigned short m_port;	//端口
	char m_type;			//客户端类型,control , compute
	int m_gputhreads;		//GPU数目
	int m_cputhreads;		//CPU数目
	
	unsigned int m_clientsock;
}client_login_req;

#define	MAX_WRITE_RETRY			15 // 重试写入文件次数
#define	MAX_SEND_BUFFER			1024 * 8 // 最大发送数据长度
#define MAX_RECV_BUFFER			1024 * 8 // 最大接收数据长度

#endif // !defined(AFX_MACROS_H_INCLUDED)