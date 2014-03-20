

#ifndef _PACKET_PROCESS_H_
#define _PACKET_PROCESS_H_

#include <stdio.h>
#include <string.h>

#define CREATE_SUP_CTL_ERROR	-1
#define CREATE_CTL_ERROR	-2
#define CREATE_CMP_ERROR	-3


//Task upload 
#define ALG_TYPE_LEN 4
#define HASH_INFO_LEN 32
#define GUID_LEN	32


//客户端登录类型
enum CLIENT_LOGIN_TYPE{

	ROLE_SUPER_CONTROL = 0x00,
	ROLE_NORMAL_CONTROL,
	ROLE_COMPUTE,
	
};

/*
//server echo command
enum CLIENT_RES_TYPE{
	
	CLIENT_LOGIN_OK,		//客户端登录成功
	CLIENT_LOGIN_ERROR,		//客户端登录失败
	COMMAND_ACTIVED,		//客户端心跳回应 
	COMMAND_REMOVE,	//客户端被动下线回应标志

};

*/
//客户端发送的数据格式为：
//	CMD_TYPE : CMD_REQ_TYPE,REQ_TYPE_LEN,REQ_CONTENT

//客户端请求标志类型
enum CMD_CONTENT_REQ_TYPE{

	//task upload req cmd
	REQ_ALG_TYPE,
	REQ_HASH_INFO,
	REQ_CTL_GUID,

	//task start req cmd
	REQ_TASK_GUID,
	REQ_TASK_PRIORITY,
	REQ_TASK_START,
	REQ_TASK_END,
	REQ_TASK_CHARSET,


	//task stop req cmd
	// use the task start cmd
	// REQ_TASK_GUID, 

	//task pause req cmd
	//REQ_TASK_GUID, 


	//task delete req cmd
	//REQ_TASK_GUID, 


	//get task result cmd
	//REQ_TASK_GUID

	//get running task status

	//get computing client list

	//computing nodes recovered req cmd
	REQ_DEC_RESULT_RECOVERED,
	REQ_COMP_NODE_GUID,

	//computing node unrecovered req cmd
	REQ_DEC_RESULT_UNRECOVERED,
	//使用和REQ_COMP_NODE_GUID 相同的GUID
	
	//computing node failed req cmd
	//使用和REQ_COMP_NODE_GUID 相同的GUID

	//computing node get workitem req cmd
	REQ_WORKITEM_GUID,
	//使用和REQ_COMP_NODE_GUID 相同的GUID

};

enum CMD_CONTENT_RES_TYPE{

	//task upload res cmd
	RES_TASK_GUID,


	//task start res cmd
	RES_TASK_START_CAUSE,


	//task stop res cmd
	RES_TASK_STOP_CAUSE,

	//task delete res cmd
	RES_TASK_DEL_CAUSE,


	//task pause res cmd
	RES_TASK_PAUSE_CAUSE,


	//get task result cmd
	RES_TASK_FINISHED,	//任务解密成功，返回解密结果
	RES_TASK_RUNNING,	//任务解密进行中，解密未完成
	RES_TASK_FINISHED_NO_RESULT,	//任务解密完成，未获得解密结果
	RES_TASK_FAILURE,		//解密失败

	RES_TASK_RESULT,	//任务解密结果

	//get running task status
	RES_TASK_STATUS,

	//get the computing client list
	RES_COMP_NODES_INFO,
	
	//返回解密完成且找到密码
	RES_WORKITEMS_GUID,   //计算节点运行结束后，将相关的工作项标识返回给计算节点


	//返回解密完成，但未找到密码
	//RES_WORKITEMS_GUID, 和上边相同

	//返回切分任务后的 工作项
	RES_WORKITEMS_INFO,



	//返回解密失败

};


INT doRecvData(LPVOID pclient, LPBYTE pdata, UINT len,BYTE cmd);


#endif


