
#ifndef _COMMON_H_
#define _COMMON_H_

//#include "SockServer.h"

//CSockServer *g_Server = NULL;

static BYTE BUFF_TAG[5]={'G','&','C','P','U'};

#define HEADER_SIZE 13
#define DEC_RESULT_SIZE	128


struct NET_BUFF_HEADER{

	BYTE tag[5];
	UINT totallen;
	UINT uncompresslen;

};

enum CMD_TYPE{

	//请求类型
	CMD_LOGIN = 0x00,
	CMD_KEEPLIVE,
	
	//控制节点发送的请求命令字
	CMD_TASK_UPLOAD,
	CMD_TASK_START,
	CMD_TASK_STOP,
	CMD_TASK_DELETE,
	CMD_TASK_PAUSE,
	CMD_TASK_RESULT,
	CMD_REFRESH_STATUS,
	CMD_GET_CLIENT_LIST,


	//计算节点发送的请求命令字
	CMD_WORKITEM_RECOVERD,
	CMD_WORKITEM_UNRECOVERED,
	CMD_WORKITEM_GET,
	CMD_WORKITEM_FAILED,

	CMD_MAX,

};


enum CMD_RES_TYPE{
	
	//请求命令字相关的应答
	CMD_LOGIN_OK,
	CMD_LOGIN_ERROR,

	CMD_KEEPLIVE_OK,
	CMD_KEEPLIVE_ERR,
	
	CMD_TASK_UPLOAD_OK,
	CMD_TASK_UPLOAD_ERR,

	CMD_TASK_START_OK,
	CMD_TASK_START_ERR,

	CMD_TASK_STOP_OK,
	CMD_TASK_STOP_ERR,

	CMD_TASK_DEL_OK,
	CMD_TASK_DEL_ERR,

	CMD_TASK_PAUSE_OK,
	CMD_TASK_PAUSE_ERR,

	CMD_TASK_RESULT_OK,
	CMD_TASK_RESULT_ERR,


	CMD_REF_STATUS_OK,
	CMD_REF_STATUS_ERR,

	CMD_GET_CLTS_OK,
	CMD_GET_CLTS_ERR,

	CMD_WI_RECOVERED_OK,
	CMD_WI_RECOVERED_ERR,

	CMD_WI_UNRECOVERED_OK,
	CMD_WI_UNRECOVERED_ERR,
	
	CMD_WI_GET_OK,
	CMD_WI_GET_ERR,
		
	CMD_WI_FAILED_OK,
	CMD_WI_FAILED_ERR,

};


//服务器应答数据结果，可以根据实际情况添加相关选项

//返回任务的guid
typedef struct _TASK_GUID_{

	GUID guid;

}TaskGuid;

typedef struct _TASK_DEC_RESULT_{

	unsigned char m_result[DEC_RESULT_SIZE];
	unsigned char m_dec_status;

}TaskResult;

//正在解密的任务状态信息,Task status info

typedef struct _TASK_STATUS_{

	GUID guid;
	float m_progress;
	
	unsigned m_split_number;
	unsigned m_fini_number;
	
	//......

}TaskStatus;

//返回的在线计算节点信息
typedef struct _COMPUTING_NODE_INFO_{

	OSVERSIONINFOEX OsVerInfoEx;
	IN_ADDR IPAddress;
	char HostName[50];
	GUID guid;
	UINT cpuThreads;
	UINT gpuThreads;
	HANDLE socket;

}CompNodeInfo;


//返回结束,工作项的唯一标识
typedef struct _WORKITEM_GUID_{

	GUID guid;

}WorkItemGuid;

//返回工作项的具体信息
typedef struct _WORKITEM_INFO_{

	GUID guid;
	BYTE wi_john[128];
	BYTE wi_begin[16];
	BYTE wi_end[16];
	BYTE wi_charset[16];
	

}WorkItemInfo;



#endif


