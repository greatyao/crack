

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


//�ͻ��˵�¼����
enum CLIENT_LOGIN_TYPE{

	ROLE_SUPER_CONTROL = 0x00,
	ROLE_NORMAL_CONTROL,
	ROLE_COMPUTE,
	
};

/*
//server echo command
enum CLIENT_RES_TYPE{
	
	CLIENT_LOGIN_OK,		//�ͻ��˵�¼�ɹ�
	CLIENT_LOGIN_ERROR,		//�ͻ��˵�¼ʧ��
	COMMAND_ACTIVED,		//�ͻ���������Ӧ 
	COMMAND_REMOVE,	//�ͻ��˱������߻�Ӧ��־

};

*/
//�ͻ��˷��͵����ݸ�ʽΪ��
//	CMD_TYPE : CMD_REQ_TYPE,REQ_TYPE_LEN,REQ_CONTENT

//�ͻ��������־����
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
	//ʹ�ú�REQ_COMP_NODE_GUID ��ͬ��GUID
	
	//computing node failed req cmd
	//ʹ�ú�REQ_COMP_NODE_GUID ��ͬ��GUID

	//computing node get workitem req cmd
	REQ_WORKITEM_GUID,
	//ʹ�ú�REQ_COMP_NODE_GUID ��ͬ��GUID

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
	RES_TASK_FINISHED,	//������ܳɹ������ؽ��ܽ��
	RES_TASK_RUNNING,	//������ܽ����У�����δ���
	RES_TASK_FINISHED_NO_RESULT,	//���������ɣ�δ��ý��ܽ��
	RES_TASK_FAILURE,		//����ʧ��

	RES_TASK_RESULT,	//������ܽ��

	//get running task status
	RES_TASK_STATUS,

	//get the computing client list
	RES_COMP_NODES_INFO,
	
	//���ؽ���������ҵ�����
	RES_WORKITEMS_GUID,   //����ڵ����н����󣬽���صĹ������ʶ���ظ�����ڵ�


	//���ؽ�����ɣ���δ�ҵ�����
	//RES_WORKITEMS_GUID, ���ϱ���ͬ

	//�����з������� ������
	RES_WORKITEMS_INFO,



	//���ؽ���ʧ��

};


INT doRecvData(LPVOID pclient, LPBYTE pdata, UINT len,BYTE cmd);


#endif


