
#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

struct control_header
{
	unsigned char magic[5];			//G&CPU
	unsigned char cmd;				//命令字
	short response;					//回应状态
	unsigned int dataLen;			//原始数据长度
	unsigned int compressLen;		//压缩数据后长度
};

#define INITIALIZE_HEADER(cmd, resp, len, len2)  {{'G', '&', 'C', 'P', 'U'}, cmd, resp, len, len2}
#define INITIALIZE_EMPTY_HEADER(cmd)  {{'G', '&', 'C', 'P', 'U'}, cmd, 0, 0, 0}

// BYTE最大也就256
enum
{
	//// 控制端发出的命令
	COMMAND_ACTIVED = 0x00,			// 服务端可以激活开始工作
	COMMAND_REMOVE,					// 程序退出
	COMMAND_REPLAY_HEARTBEAT,		// 回复心跳包
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



#define	MAX_WRITE_RETRY			15 // 重试写入文件次数
#define	MAX_SEND_BUFFER			1024 * 8 // 最大发送数据长度
#define MAX_RECV_BUFFER			1024 * 8 // 最大接收数据长度

#endif // !defined(AFX_MACROS_H_INCLUDED)