

#include "stdafx.h"
#include "PacketProcess.h"
#include "CLog.h"
#include "Common.h"
#include "zlib.h"
#include "ServerResp.h"
#include "guidgenerator.h"
#include "macros.h"
#include "algorithm_types.h"

#include "ReqPacket.h"
#include "ResPacket.h"
#include "CrackBroker.h"
#include <stdio.h>


#define FILE_DIR ".\\tempdir\\"


static CCrackBroker g_CrackBroker;

int getClientIPInfo(void *pclient,char *pip,int *port){
	
	SOCKET clisock = *(SOCKET *)pclient;

	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	getpeername(clisock, (sockaddr *)&addr, &addrlen);

	strcpy(pip, inet_ntoa(addr.sin_addr));
	*port = ntohs(addr.sin_port);

	return 0;
}
//recv file upload data
int doRecvDataNoCompress(void *pClient,unsigned char *pdata,unsigned int len){
	
	int nRet = 0;
	LPBYTE pSendBuf = NULL;
	UINT nTotal = 0;
	UINT nCompressLen = 0;
	unsigned long lcomlen = 0;

	BYTE sendBuf[MAX_BUF_LEN];
	control_header recvhdr;
	unsigned long lOrgLen = 0;
	unsigned long lCompLen = 0;
	control_header replyHdr = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
	unsigned int cltHdrLen = 0;
	cltHdrLen = sizeof(control_header);
	
	if (cltHdrLen > len){

		CLog::Log(LOG_LEVEL_WARNING,"Reply Len Must Bigger than Control Header\n");
		return -1;

	}

			
	if (cltHdrLen == len){

		CopyMemory(&replyHdr,pdata,cltHdrLen);
		nRet = RecvDataFromPeer(pClient,pdata, cltHdrLen);
		
		if (nRet < 0){
			
			CLog::Log(LOG_LEVEL_WARNING,"Recv Req Header Error\n");
			return -1;

		}


		CLog::Log(LOG_LEVEL_WARNING,"Recv Only Contain a Control Header OK\n");
		return 0;
	}

	//lOrgLen = len - sizeof(control_header);

	CLog::Log(LOG_LEVEL_WARNING,"Recv Contain a buffer\n");

	nRet = RecvDataFromPeer(pClient,(unsigned char *)&recvhdr, cltHdrLen);
	
	if (nRet < 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Recv Reply Header Error\n");
		return -1;

	}

	lOrgLen = recvhdr.dataLen;
	nRet = RecvDataFromPeer(pClient, pdata, lOrgLen);
	if (nRet < 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Recv Data Error\n");
		return -3;

	}


//	CLog::Log(LOG_LEVEL_WARNING,"Send Data OK\n");
	return nRet;
}

//完成数据的组装,包含两部分，一部分为固定长度包头，另一部分为包内容
int doSendDataNew(void *pClient,unsigned char *pdata,unsigned int len){

	int nRet = 0;
	LPBYTE pSendBuf = NULL;
	UINT nTotal = 0;
	UINT nCompressLen = 0;
	unsigned long lcomlen = 0;

	BYTE sendBuf[MAX_BUF_LEN];
	unsigned long lOrgLen = 0;
	unsigned long lCompLen = 0;
	control_header replyHdr = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
	unsigned int cltHdrLen = 0;
	cltHdrLen = sizeof(control_header);
	
	if (cltHdrLen > len){

		CLog::Log(LOG_LEVEL_WARNING,"Reply Len Must Bigger than Control Header\n");
		return -1;

	}

			
	if (cltHdrLen == len){

		CopyMemory(&replyHdr,pdata,cltHdrLen);
		nRet = SendDataToPeer(pClient, (unsigned char *)&replyHdr, cltHdrLen);
		
		if (nRet < 0){
			
			CLog::Log(LOG_LEVEL_WARNING,"Send Reply Header Error\n");
			return -1;

		}


		CLog::Log(LOG_LEVEL_WARNING,"Reply Only Contain a Control Header OK\n");
		return 0;
	}


	CLog::Log(LOG_LEVEL_WARNING,"Reply Contain a buffer\n");

	lOrgLen =len - cltHdrLen;
	ZeroMemory(sendBuf,MAX_BUF_LEN);

	//计算获得发送数据长度
	lCompLen = MAX_BUF_LEN;

	nRet = compress(sendBuf,&lCompLen,pdata+cltHdrLen,lOrgLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -2;

	}
		
	//调用发送接口，发送给对端

	/*
		short response;					//回应状态
	unsigned int dataLen;			//原始数据长度
	unsigned int compressLen;		//压缩数据后长度
	*/
	
	CopyMemory(&replyHdr,pdata,cltHdrLen);
	replyHdr.response = 0;
	replyHdr.dataLen = lOrgLen;
	replyHdr.compressLen = lCompLen;

	nRet = SendDataToPeer(pClient,(unsigned char *)&replyHdr, cltHdrLen);
	
	if (nRet < 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Send Reply Header Error\n");
		return -1;

	}

	nRet = SendDataToPeer(pClient, sendBuf, lCompLen);
	if (nRet < 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Send Data Error\n");
		return -3;

	}


//	CLog::Log(LOG_LEVEL_WARNING,"Send Data OK\n");
	return 0;


}

//完成数据的组装,包含两部分，一部分为固定长度包头，另一部分为包内容
int doSendDataNoCompress(void *pClient,unsigned char *pdata,unsigned int len){

	int nRet = 0;
	LPBYTE pSendBuf = NULL;
	UINT nTotal = 0;
	UINT nCompressLen = 0;
	unsigned long lcomlen = 0;

	BYTE sendBuf[MAX_BUF_LEN];
	unsigned long lOrgLen = 0;
	unsigned long lCompLen = 0;
	control_header replyHdr = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
	unsigned int cltHdrLen = 0;
	cltHdrLen = sizeof(control_header);
	
	if (cltHdrLen > len){

		CLog::Log(LOG_LEVEL_WARNING,"Reply Len Must Bigger than Control Header\n");
		return -1;

	}

			
	if (cltHdrLen == len){

		CopyMemory(&replyHdr,pdata,cltHdrLen);
		nRet = SendDataToPeer(pClient, (unsigned char *)&replyHdr, cltHdrLen);
		
		if (nRet < 0){
			
			CLog::Log(LOG_LEVEL_WARNING,"Send Reply Header Error\n");
			return -1;

		}


		CLog::Log(LOG_LEVEL_WARNING,"Reply Only Contain a Control Header OK\n");
		return 0;
	}


	CLog::Log(LOG_LEVEL_WARNING,"Reply Contain a buffer\n");

	lOrgLen =len - cltHdrLen;
	ZeroMemory(sendBuf,MAX_BUF_LEN);

	//计算获得发送数据长度
/*	lCompLen = MAX_BUF_LEN;

	nRet = compress(sendBuf,&lCompLen,pdata+cltHdrLen,lOrgLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -2;

	}
	*/	
	//调用发送接口，发送给对端

	/*
		short response;					//回应状态
	unsigned int dataLen;			//原始数据长度
	unsigned int compressLen;		//压缩数据后长度
	*/
	
	CopyMemory(&replyHdr,pdata,cltHdrLen);
	replyHdr.response = 0;
	replyHdr.dataLen = lOrgLen;
	replyHdr.compressLen = -1;

	nRet = SendDataToPeer(pClient,(unsigned char *)&replyHdr, cltHdrLen);
	
	if (nRet < 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Send Reply Header Error\n");
		return -1;

	}

	nRet = SendDataToPeer(pClient, pdata+sizeof(control_header), lOrgLen);
	if (nRet < 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Send Data Error\n");
		return -3;

	}


//	CLog::Log(LOG_LEVEL_WARNING,"Send Data OK\n");
	return 0;


}






//完成数据的组装
int doSendData(void *pClient,unsigned char *pdata,unsigned int len){

	int nRet = 0;
	LPBYTE pSendBuf = NULL;
	UINT nTotal = 0;
	UINT nCompressLen = 0;
	unsigned long lcomlen = 0;

	//计算获得发送数据长度
	nCompressLen = (float)len * 1.001+12;

	pSendBuf = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,HEADER_SIZE+nCompressLen);
	if (!pSendBuf){

		CLog::Log(LOG_LEVEL_WARNING,"Create Send Buffer Error\n");
		return -1;
	}
	
	nRet = compress(&pSendBuf[HEADER_SIZE],&lcomlen,pdata,len);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSendBuf);
		return -2;

	}
	nCompressLen = lcomlen;

	nTotal = nCompressLen + HEADER_SIZE;
	
	CopyMemory(pSendBuf,BUFF_TAG,5);
	CopyMemory(&pSendBuf[5],&nTotal,4);
	CopyMemory(&pSendBuf[9],&nCompressLen,4);

	
	//调用发送接口，发送给对端
	nRet = SendDataToPeer(pClient, pSendBuf, nTotal);
	if (nRet != 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Send Data Error\n");
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSendBuf);
		return -3;

	}


	CLog::Log(LOG_LEVEL_WARNING,"Send Data OK\n");
	return 0;


}


//得到Workitem status
int comp_wi_status(void *pclient, unsigned char * pdata, UINT len){

	int ret = 0;
	CLog::Log(LOG_LEVEL_WARNING,"this is get workitem status\n");
	
	crack_status* status = (crack_status*)(pdata);
	CLog::Log(LOG_LEVEL_WARNING,"%s: 进度%d 速度%f 时间%u\n", status->guid, status->progress, status->speed, status->remainTime);
			
	return ret;
}


//得到workitem result
int comp_wi_result(void *pclient, unsigned char * pdata, UINT len){

	int ret = 0;
	CLog::Log(LOG_LEVEL_WARNING,"this is Get a WorkItem Result\n");
	

	crack_result* result = (crack_result*)(pdata);
	if(result->status == WORK_ITEM_UNLOCK)
	{
		CLog::Log(LOG_LEVEL_WARNING,"对%s解锁，让其他计算节点使用\n", result->guid);
	}
	else if(result->status == WORK_ITEM_WORKING)
	{
		CLog::Log(LOG_LEVEL_WARNING,"%s已经被该计算节点解密了，请等待结果\n", result->guid);
	}
	else if(result->status == WORK_ITEM_CRACKED)
	{
		CLog::Log(LOG_LEVEL_WARNING,"%s: 解密码 %s\n", result->guid, result->password);
	}
	else 
	{
		CLog::Log(LOG_LEVEL_WARNING,"%s: 未解出密码\n", result->guid);
	}

	return ret;
}


///new control client task req for test

/*
struct client_login_req {
	
	char m_osinfo[16];	//操作系统信息
	char m_ip[20];		//IP地址信息

	char m_type;		//客户端类型,control , compute
	
	char m_hostname[50];//主机名称
	unsigned char m_guid[40]; //节点guid
	
	unsigned int m_clientsock;
	int m_gputhreads;
	int m_cputhreads;
	*/

int client_loginnew(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	char buf[40];
	char ip[20];
	int port = 0;
	control_header cltHeader = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
	CLog::Log(LOG_LEVEL_WARNING,"Enter into Login\n");

	client_login_req *pC = (struct client_login_req *)pdata;
	client_login_req myclient;

	getClientIPInfo(pclient,ip,&port);

	if (pclient == NULL){
		
		memset(&myclient,0,sizeof(struct client_login_req));
		memset(buf,0,40);
		sprintf(buf,"%u",*(SOCKET *)pclient);
		myclient.m_clientsock = *(SOCKET *)pclient;
		myclient.m_cputhreads = myclient.m_gputhreads = 0;
		myclient.m_type = 0;
//		memcpy(myclient.m_guid,buf,strlen(buf));
		memcpy(myclient.m_ip,ip,16);

		pC = &myclient;
		
	}

	//处理业务逻辑

	nRet = g_CrackBroker.ClientLogin(pC);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Client Login IP %s port %d Error\n",ip,port);
	

	}else {
		
		CLog::Log(LOG_LEVEL_WARNING,"Client Login IP %s port %d OK\n",ip,port);

	}


	////////////////////////////////////

	//产生应答报文，并发送
	cltHeader.response = nRet;
		 
    nRet = doSendDataNew(pclient,(unsigned char *)&cltHeader,sizeof(control_header));
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Login Error\n",ip,port);
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Login OK\n",ip,port);
		nRet = 0;
	}

	return nRet;

}

//同步心跳回应
int client_keeplivenew(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	control_header replyHdr = INITIALIZE_EMPTY_HEADER(COMMAND_REPLAY_HEARTBEAT);
	CLog::Log(LOG_LEVEL_WARNING,"This is a clieng keeplive\n");
	struct client_keeplive_req *pKeeplive = (struct client_keeplive_req *)pdata;
	struct client_keeplive_req keeplive;

	char buf[40];
	//GUID guid;
	//INT guidLen = 0;
	time_t tmpTime = 0;

	//BYTE bToken = CMD_KEEPLIVE_OK;
	//guidLen = sizeof(GUID);
	//CopyMemory(&guid,&pdata[1],guidLen);
	//CopyMemory(&tmpTime,&pdata[1+guidLen],sizeof(time_t));
	
	//处理业务逻辑
	if (pKeeplive == NULL){
				
		memset(buf,0,40);
		sprintf(buf,"%u",*(SOCKET *)pclient);
		memcpy(keeplive.m_guid,buf,strlen(buf));
		pKeeplive = &keeplive;

	}
	
	nRet = g_CrackBroker.ClientKeepLive(pKeeplive);



	replyHdr.response = 0;
	replyHdr.dataLen = 0;

	//生成应答并返回
	
	nRet = doSendDataNew(pclient,(unsigned char *)&replyHdr,sizeof(control_header));
//	nRet = doSendData(pclient, &bToken, 1);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Client KeepLive Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Client KeepLive OK\n");
		nRet = 0;
	}
	return nRet;
}


int cc_task_uploadNew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task upload New\n");
	int nRet = 0;

	UINT sendLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned long lcomplen = 0;
	unsigned long luncomplen = 0;
	unsigned int resLen = 0;

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_UPLOAD);
	crack_task *pCrackTask = NULL;
	task_upload_res task_upload;
	char c_guid[48];

	memset(resBuf,0,MAX_BUF_LEN);
	
	if (len != sizeof(crack_task)){

		CLog::Log(LOG_LEVEL_WARNING,"Upload Task : Data len no crack_task size Error\n");
		return -2;

	}


	pCrackTask = (crack_task *)pdata;

	CLog::Log(LOG_LEVEL_WARNING,"task Task status info charset : %d, filename : %s,algo : %d\n",pCrackTask->charset,pCrackTask->filename,pCrackTask->algo);

		
	memset(resBuf,0,MAX_BUF_LEN);
	//memcpy(task_upload.guid,"9876543210987654321098765432109876543210",40);
	
	nRet = g_CrackBroker.CreateTask(pCrackTask,task_upload.guid);
	if (nRet < 0) {

		CLog::Log(LOG_LEVEL_WARNING,"Upload Task : Create Task Error\n");
		resLen = 0;
	}else{
		
		CLog::Log(LOG_LEVEL_WARNING,"Upload Task : Create Task OK\n");
		resLen = sizeof(struct task_upload_res);
	}
	
	//业务处理

	//产生应答报文，并发送
	
	reshdr.dataLen = resLen;
	reshdr.response = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(control_header));
	
	if (resLen != 0){
		memcpy(resBuf+sizeof(control_header),&task_upload,resLen);
	}
	
	
	sendLen = sizeof(control_header)+resLen;


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Upload Task Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Upload Task OK\n");
		nRet = 0;
	}

	return nRet;
}


int cc_task_startnew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task start new\n");
	
	int nRet = 0;

	UINT sendLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned long lcomplen = 0;
	unsigned long luncomplen = 0;
	unsigned int resLen = 0;

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_START);
	task_start_req *pStartReq = NULL;
	task_status_res taskres;
	char c_guid[48];

	//

	memset(resBuf,0,MAX_BUF_LEN);
	
	
	if (len != sizeof(task_start_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Start Task : Data len not task_start_req size Error\n");
		return -2;

	}

	pStartReq = (task_start_req *)pdata;

	printf("Start Task guid : %s\n",pStartReq->guid);

		
	memset(resBuf,0,MAX_BUF_LEN);
	
	memcpy(taskres.guid,pStartReq->guid,strlen((char *)pStartReq->guid));
	taskres.status = 100;

	//业务处理
	nRet = g_CrackBroker.StartTask(pStartReq);
	if (nRet < 0 ){

		CLog::Log(LOG_LEVEL_WARNING,"Start Task Error\n");
		resLen = 0;


	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Start Task OK\n");
		resLen = sizeof(task_status_res);
		

	}

taskres.status= nRet;
	//产生应答报文，并发送
	reshdr.dataLen = resLen;
	reshdr.response = nRet;

	memcpy(resBuf,&reshdr,sizeof(control_header));
	if (resLen != 0)
		memcpy(resBuf+sizeof(control_header),&taskres,resLen);
	
	sendLen = sizeof(control_header)+resLen;


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Start Task Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Start Task OK\n");
		nRet = 0;
	}

	return nRet;

}

//stop task

int cc_task_stopnew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task Stop new\n");
	
	int nRet = 0;
	UINT sendLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned long lcomplen = 0;
	unsigned long luncomplen = 0;
	unsigned int resLen = 0;

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_STOP);
	struct task_stop_req *pStopReq = NULL;
	struct task_status_res taskres;
	char c_guid[48];

	//

	memset(resBuf,0,MAX_BUF_LEN);
	
	
	if (len != sizeof(task_stop_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task : Data len not task_stop_req size Error\n");
		return -2;

	}

	pStopReq = (struct task_stop_req *)pdata;

	CLog::Log(LOG_LEVEL_WARNING,"Stop Task guid : %s\n",pStopReq->guid);

		
	memset(resBuf,0,MAX_BUF_LEN);
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pStopReq->guid,strlen((char *)pStopReq->guid));


	//业务处理
	nRet = g_CrackBroker.StopTask(pStopReq);
	if (nRet < 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"Stop Task Error\n");
		resLen = 0;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task OK\n");
		resLen = sizeof(task_status_res);
	
	}
	taskres.status = nRet;

	//产生应答报文，并发送
	reshdr.dataLen = resLen;
	reshdr.response = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(control_header));
	if (resLen != 0)
		memcpy(resBuf+sizeof(control_header),&taskres,resLen);
	
	sendLen = sizeof(control_header)+resLen;


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Stop Task Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Stop Task OK\n");
		nRet = 0;
	}

	return nRet;

}


//pause task

int cc_task_pausenew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task pause new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	UINT sendLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_PAUSE);
	task_pause_req *pPauseReq = NULL;
	task_status_res taskres;


	memset(resBuf,0,MAX_BUF_LEN);
	
	
	if (len != sizeof(task_pause_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task : Data len not task_pause_req size Error\n");
		return -2;

	}

	pPauseReq = (task_pause_req *)pdata;

	CLog::Log(LOG_LEVEL_WARNING,"Pause Task guid : %s\n",pPauseReq->guid);

		
	memset(resBuf,0,MAX_BUF_LEN);
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pPauseReq->guid,strlen((char *)pPauseReq->guid));

	//处理业务逻辑
	nRet = g_CrackBroker.PauseTask(pPauseReq);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Pause Task %s ErrorCode : %d\n",pPauseReq->guid,nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Broker Pause Task %s OK\n",pPauseReq->guid);
		resLen = sizeof(struct task_status_res);

	}

	//产生应答报文，并发送
	reshdr.dataLen = resLen;
	taskres.status = nRet;
	reshdr.response = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(control_header));

	if (resLen != 0)
		memcpy(resBuf+sizeof(control_header),&taskres,resLen);
	
	sendLen = sizeof(control_header)+resLen;


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Pause Task Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Pause Task OK\n");
		nRet = 0;
	}

	return nRet;

}

//delete task

int cc_task_deletenew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task Delete new\n");
	
	int nRet = 0;

	unsigned int resLen = 0;
	UINT sendLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_DELETE);
	task_delete_req *pDeleteReq = NULL;
	task_status_res taskres;


	//
	memset(&taskres,0,sizeof(struct task_status_res));
	memset(resBuf,0,MAX_BUF_LEN);
	
	
	if (len != sizeof(task_delete_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Delete Task : Data len not task_delete_req size Error\n");
		return -2;

	}

	pDeleteReq = (task_delete_req *)pdata;

	CLog::Log(LOG_LEVEL_WARNING,"Delete Task guid : %s\n",pDeleteReq->guid);

		
	memcpy(taskres.guid,pDeleteReq->guid,strlen((char *)pDeleteReq->guid));

	//处理业务逻辑
	nRet = g_CrackBroker.DeleteTask(pDeleteReq);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Delete Task %s ErrorCode : %d\n",pDeleteReq->guid,nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Broker Delete Task %s OK\n",pDeleteReq->guid);
		resLen = sizeof(struct task_status_res);

	}

	//产生应答报文，并发送
	reshdr.dataLen = resLen;
	reshdr.response = nRet;
	taskres.status = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(control_header));
	if (resLen != 0)
		memcpy(resBuf+sizeof(control_header),&taskres,sizeof(task_status_res));
	
	sendLen = sizeof(control_header)+sizeof(task_status_res);

	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Delete Task Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Delete Task OK\n");
		nRet = 0;
	}

	return nRet;

}


//get a task result,动态开辟空间
int cc_get_task_resultnew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is Get atask Result new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	UINT sendLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned long lcomplen = 0;
	unsigned long luncomplen = 0;

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_RESULT);
	task_result_req *pResReq = NULL;
	task_status_res *pres = NULL;

	memset(resBuf,0,MAX_BUF_LEN);
	
	if (len != sizeof(task_result_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Get A Task Result: Data len not task_result_req size Error\n");
		return -2;

	}

	pResReq = (task_result_req *)pdata;

	CLog::Log(LOG_LEVEL_WARNING,"Get a Task result guid : %s\n",pResReq->guid);
	
	//处理业务逻辑
	nRet = g_CrackBroker.GetTaskResult(pResReq,&pres);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task %s Result ,ErrorCode : %d\n",pResReq->guid,nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task %s Result OK\n",pResReq->guid);
		resLen = sizeof(struct task_status_res);

	}


	//产生应答报文，并发送
	reshdr.dataLen = sizeof(task_status_res);
	reshdr.response = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(control_header));
	memcpy(resBuf+sizeof(control_header),pres,sizeof(task_status_res));


	sendLen = sizeof(control_header)+sizeof(task_status_res);


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Get A Task Result Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Get A Task Result OK\n");
		nRet = 0;
	}

	g_CrackBroker.Free(pres);
	return nRet;

}


//get task status，动态开辟空间
int cc_refresh_statusnew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is Refresh Task Status new\n");
	
	int nRet = 0;
	int i = 0;
	UINT sendLen = 0;
	unsigned int resLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];

	struct control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_REFRESH_STATUS);
	struct task_status_info *pTasksStatus = NULL;
	unsigned int resNum = 0;

	memset(resBuf,0,MAX_BUF_LEN);
	memset(&reshdr,0,sizeof(struct control_header));
	
	/*
	pTasksStatus = (task_status_info *)malloc(sizeof(task_status_info)*2);

	if (!pTasksStatus){

		CLog::Log(LOG_LEVEL_WARNING,"Refresh Task Status Malloc Error\n");
		return -1;

	}

	pCur = pTasksStatus;

	memcpy((char *)pCur->guid,"10000",5);
	pCur->m_fini_number = 10;
	pCur->m_split_number = 100;
	pCur->m_progress = 10.0;

	pCur->status = 32;

	pCur = pTasksStatus + 1;

	memcpy((char *)pCur->guid,"abcdef",6);
	pCur->m_fini_number = 5;
	pCur->m_split_number = 29;
	pCur->m_progress = 53.0;

	pCur->status = 21;
	*/
	//处理业务逻辑
	nRet = g_CrackBroker.GetTasksStatus(&pTasksStatus,&resNum);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task Status %d Result ,ErrorCode : %d\n",resNum,nRet);
	
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task Status %d Result OK\n",resNum);
		resLen = sizeof(struct task_status_info) * resNum;
	}
	

	//产生应答报文，并发送
	reshdr.dataLen = resLen;
	reshdr.response = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(control_header));
	if (resLen != 0)
		memcpy(resBuf+sizeof(control_header),(unsigned char *)pTasksStatus,resLen);

	sendLen = sizeof(control_header)+resLen;


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Get Task Status Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result OK\n");
		nRet = 0;
	}

	g_CrackBroker.Free(pTasksStatus);
	return nRet;

}


//get client list, 动态开辟空间
int cc_get_client_listnew(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is Get Client list new\n");
	
	int nRet = 0;
	UINT sendLen = 0;
	unsigned int resLen = 0;
	unsigned char resBuf[MAX_BUF_LEN];

	struct control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_GET_CLIENT_LIST);
	struct compute_node_info *pClients = NULL;
	unsigned int resNum = 0;

	memset(&reshdr,0,sizeof(struct control_header));
	memset(resBuf,0,MAX_BUF_LEN);
	/*
	pClients = (compute_node_info *)malloc(sizeof(compute_node_info)*2);
	if (!pClients){
	
		CLog::Log(LOG_LEVEL_WARNING,"Get Compute Clients Malloc Error\n");
		return -2;


	}

	memset(pClients,0,sizeof(compute_node_info)*2);
	pCur = pClients;

	pCur->cputhreads = 2;
	pCur->gputhreads = 10;
	strcpy((char *)pCur->ip,"192.168.30.20");
	strcpy((char *)pCur->hostname,"testcom");
	strcpy((char *)pCur->guid,"guid0001");
	
	strcpy((char *)pCur->os,"win7");

	pCur = pClients+1;

	pCur->cputhreads = 30;
	pCur->gputhreads = 23;
	strcpy((char *)pCur->ip,"192.168.200.12");
	strcpy((char *)pCur->hostname,"computernode1");
	strcpy((char *)pCur->guid,"0002");
	
	strcpy((char *)pCur->os,"ubuntu");
	*/

	//处理业务逻辑
	nRet = g_CrackBroker.GetClientList(&pClients,&resNum);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Clients ,number is %d ,ErrorCode : %d\n",resNum,nRet);
	
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Clients ,number is %d OK\n",resNum);
		resLen = sizeof(struct compute_node_info) * resNum;
	}
	


	//产生应答报文，并发送
	reshdr.dataLen = resLen;
	reshdr.response = nRet;
	
	memcpy(resBuf,&reshdr,sizeof(struct control_header));
	memcpy(resBuf+sizeof(struct control_header),(unsigned char *)pClients,resLen);
	
	sendLen = sizeof(struct control_header)+resLen;


	nRet = doSendDataNew(pclient, resBuf, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Get Compute Clients Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Get Compute Clients OK\n");
		nRet = 0;
	}

	g_CrackBroker.Free(pClients);
	return nRet;

}



//download file res
// download file 
int cc_task_file_download_start(void *pclient, unsigned char * pdata,UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is file download start \n");
	int ret = 0;
	FILE *pfile = NULL;
	unsigned int sendLen = 0;
	BYTE resBuf[MAX_BUF_LEN];
	file_info fileinfo;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_DOWNLOAD_FILE);
	unsigned int filelen =  0;
	unsigned char guid[40];


	memcpy(guid,pdata,len);

	pfile = fopen((char *)guid,"rb");
	if (!pfile){
		
		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s error \n",guid);
		return -1;

	}
	

	fseek(pfile,0L,SEEK_END);
	filelen = ftell(pfile);  //获取文件长度
	fseek(pfile,0L,SEEK_SET);
			
	fileinfo.f = pfile;
	fileinfo.len = filelen;
	fileinfo.offset = 0;

	CLog::Log(LOG_LEVEL_WARNING,"pfile %p,len : %d ,offset:%d, guid : %s \n",fileinfo.f,fileinfo.len,fileinfo.offset,guid);

	reshdr.dataLen = sizeof(file_info);

	memset(resBuf,0,MAX_BUF_LEN);

	memcpy(resBuf,&reshdr,sizeof(control_header));
	memcpy(resBuf+sizeof(control_header),&fileinfo,sizeof(file_info));
	
	sendLen = sizeof(control_header)+sizeof(file_info);


	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"file download start Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"file download start OK\n");
		ret = 0;
	}
	
	return ret;
}



//download file res tran
//download file start
int cc_task_file_download(void *pclient,unsigned char *pdata,UINT len){
	
	CLog::Log(LOG_LEVEL_WARNING,"this is file download ...\n");
	int ret = 0;
	int rdlen = 0;
	int readLen = 0;
	FILE *pfile = NULL;
	unsigned int sendLen = 0;
	BYTE resBuf[MAX_BUF_LEN];
	file_info *pFileInfo = NULL;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_START_DOWNLOAD);
	unsigned int filelen =  0;
	unsigned char guid[40];

	
	pFileInfo = (file_info *)pdata;

	pfile = (FILE *)pFileInfo->f;


	CLog::Log(LOG_LEVEL_WARNING,"pfile %p,len : %d ,offset:%d \n",pFileInfo->f,pFileInfo->len,pFileInfo->offset);


	/*ret = fseek(pfile,pFileInfo->offset,SEEK_SET);
	if (ret!= 0){


		CLog::Log(LOG_LEVEL_WARNING,"fssek ret:%d \n",ret);

		return -1;

	}

	*/

	if (pFileInfo->len > 8196){
		
		rdlen = 8196;

	}else{
		rdlen = pFileInfo->len;

	}

	while(!feof(pfile)){
		memset(resBuf,0,MAX_BUF_LEN);

		readLen  = fread(resBuf+sizeof(control_header),1,rdlen,pfile);

		//CLog::Log(LOG_LEVEL_WARNING,"file read %d %d\n",readLen,rdlen);

		if (readLen < 0){

			CLog::Log(LOG_LEVEL_WARNING,"file read error.\n");
			return -2;

		}
				
		reshdr.dataLen = readLen;
		reshdr.compressLen = 0;

		memcpy(resBuf,&reshdr,sizeof(control_header));
		
		sendLen = sizeof(control_header)+readLen;


		ret = doSendDataNoCompress(pclient, resBuf, sendLen);
		if (ret != 0){
			CLog::Log(LOG_LEVEL_WARNING,"file download .... Error\n");
			ret = -2;
		}else{
			CLog::Log(LOG_LEVEL_WARNING,"file download .... OK\n");
			ret = 0;
		}
	
	}
	return ret;

}


//download file res end
//download file end
int cc_task_file_download_end(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is file download end\n");
	int ret = 0;
	int readLen = 0;
	FILE *pfile = NULL;
	unsigned int sendLen = 0;
	BYTE resBuf[128];
	file_info *pFileInfo = NULL;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_END_DOWNLOAD);
	unsigned int filelen =  0;
	unsigned char guid[40];

	
	pFileInfo = (file_info *)pdata;

	pfile = (FILE *)pFileInfo->f;

	fclose(pfile);


	memset(resBuf,0,128);

	memcpy(resBuf,&reshdr,sizeof(control_header));
	
	sendLen = sizeof(control_header);


	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"file download End. Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"file download End OK\n");
		ret = 0;
	}
	
	return ret;

}

//control client : upload start

//upload file req  
int cc_task_upload_file(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is file upload start \n");
	int ret = 0;
	FILE *pfile = NULL;
	unsigned int sendLen = 0;
	BYTE resBuf[MAX_BUF_LEN];
	file_upload_res uploadres;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_UPLOAD_FILE);
	unsigned int filelen =  0;
	unsigned char guid[40];


	memcpy(guid,pdata,len);
/*
	pfile = fopen((char *)guid,"ab");
	if (!pfile){
		
		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s error \n",guid);
		return -1;

	}
	*/

	memcpy(uploadres.guid,guid,40);
	uploadres.f = NULL;
	uploadres.len = 0;
	uploadres.offset = 0;

	CLog::Log(LOG_LEVEL_WARNING,"pfile %p,len : %d ,offset:%d, guid : %s \n",uploadres.f,uploadres.len,uploadres.offset,uploadres.guid);

	reshdr.dataLen = sizeof(file_upload_res);

	memset(resBuf,0,MAX_BUF_LEN);

	memcpy(resBuf,&reshdr,sizeof(control_header));
	memcpy(resBuf+sizeof(control_header),&uploadres,sizeof(file_upload_res));
	
	sendLen = sizeof(control_header)+sizeof(file_upload_res);


	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"file upload start Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"file upload start OK\n");
		ret = 0;
	}
	
	return ret;
}



//upload file start req  
int cc_task_upload_file_start(void *pclient,unsigned char *pdata,UINT len){
	
	CLog::Log(LOG_LEVEL_WARNING,"this is file upload .... \n");
	int ret = 0;
	FILE *pfile = NULL;
	unsigned int sendLen = 0;
	BYTE resBuf[MAX_BUF_LEN];
	file_upload_start_res startres;

	file_upload_end_res endres;
	file_upload_start_req *puploadstartreq = NULL;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_START_UPLOAD);
	unsigned int filelen =  0;
	unsigned int recvLen = 0;
	unsigned int curlen = 0;
	unsigned char guid[40];
	unsigned char filename[128];


	puploadstartreq = (file_upload_start_req *)pdata;

	filelen = puploadstartreq->len;


	memset(filename,0,128);
	sprintf((char *)filename,"%s%s",FILE_DIR,(char *)puploadstartreq->guid);


	pfile = fopen((char *)puploadstartreq->guid,"w");
	if (!pfile){
		
		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s error \n",guid);
		return -1;

	}


	CLog::Log(LOG_LEVEL_WARNING,"pfile %p,len : %d ,offset:%d, guid : %s \n",puploadstartreq->f,puploadstartreq->len,puploadstartreq->offset,puploadstartreq->guid);


	startres.f = pfile;
	startres.len = 1024;
	startres.offset = 0;
	memcpy(startres.guid,puploadstartreq->guid,40);
	
	reshdr.dataLen = sizeof(file_upload_start_res);

	memset(resBuf,0,MAX_BUF_LEN);

	memcpy(resBuf,&reshdr,sizeof(control_header));
	memcpy(resBuf+sizeof(control_header),&startres,sizeof(file_upload_start_res));
	
	sendLen = sizeof(control_header)+sizeof(file_upload_start_res);


	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"file upload .... Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"file upload ... OK\n");
		ret = 0;
	}
	

	//recv the file data 

	memset(resBuf,0,MAX_BUF_LEN);
	recvLen = startres.len+sizeof(control_header);

	while(curlen < filelen){

		ret = doRecvDataNoCompress(pclient,resBuf,recvLen);
		if (ret < 0 ){

			CLog::Log(LOG_LEVEL_WARNING,"Recv file data error OK\n");
			ret = -3;
			break;

		}
		curlen+=ret;
		
		CLog::Log(LOG_LEVEL_WARNING,"Recv file data %d vs %d OK\n",curlen,filelen);
		fwrite(resBuf,1,ret,pfile);
				

	}

	fclose(pfile);

	//reply a upload end 
	endres.f = pfile;
	endres.len = 0;
	endres.offset = 0;
	memcpy(endres.guid,puploadstartreq->guid,40);
	
	reshdr.cmd = CMD_END_UPLOAD;
	reshdr.dataLen = sizeof(file_upload_end_res);

	memset(resBuf,0,MAX_BUF_LEN);

	memcpy(resBuf,&reshdr,sizeof(control_header));
	memcpy(resBuf+sizeof(control_header),&endres,sizeof(file_upload_end_res));
	
	sendLen = sizeof(control_header)+sizeof(file_upload_end_res);


	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"file upload End Res Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"file upload End Res OK\n");
		ret = 0;
	}
	return ret;

}




//计算节点请求处理
/*
	CMD_GET_A_WORKITEM,		//获取一条任务的分解项 WORKITEM
	CMD_WORKITEM_STATUS,	//计算单元上报解密状态
	CMD_WORKITEM_RESULT,	//计算单元上报解密结果
	*/

int comp_get_a_workitem_new(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_block *pcrackblock = NULL;
	struct control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_GET_A_WORKITEM);
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned int resLen = 0;
	unsigned int sendLen = 0;
	int sck = *((int*)pclient);

	memset(resBuf,0,MAX_BUF_LEN);

	

	ret = g_CrackBroker.GetAWorkItem(&pcrackblock);
	if (ret < 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Error\n");
		reshdr.dataLen = 0;
		reshdr.response = ret;
		resLen = 0;
	
	}else {
		
		CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem OK\n");
		resLen = sizeof(struct crack_block);
		reshdr.dataLen = resLen;
		reshdr.response = 0;
	}
	/*
	memcpy(resBuf,&reshdr,sizeof(struct control_header));
	memcpy(resBuf+sizeof(struct control_header) ,pcrackblock,resLen);
	sendLen = sizeof(struct control_header) + resLen;
	
	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Get A Task Workitem Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Get A Task WorkItem OK\n");
		ret = 0;
	}
	*/

	int m = Write(sck, CMD_GET_A_WORKITEM, 0, pcrackblock, sizeof(*pcrackblock));
	CLog::Log(LOG_LEVEL_WARNING,"Get A Task Workitem %d\n", m);
	
	g_CrackBroker.Free(pcrackblock);

	return ret;
}

int comp_get_workitem_status_new(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_status *pstatus = NULL;
	struct control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_WORKITEM_STATUS);
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned int sendLen = 0;

	memset(resBuf,0,MAX_BUF_LEN);

	
	pstatus = (struct crack_status *)pdata;

	ret = g_CrackBroker.GetWIStatus(pstatus);
	if (ret < 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Status Error\n");
	

	}else {
		
		CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Status OK\n");
	}
	
	reshdr.dataLen = 0;
	reshdr.response = ret;
	
	memcpy(resBuf,&reshdr,sizeof(struct control_header));
	sendLen = sizeof(struct control_header);
	
	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Get Workitem Status Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Status OK\n");
		ret = 0;
	}

	return ret;
}


int comp_get_workitem_res_new(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_result *pres = NULL;
	struct control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_WORKITEM_RESULT);
	unsigned char resBuf[MAX_BUF_LEN];
	unsigned int sendLen = 0;

	memset(resBuf,0,MAX_BUF_LEN);

	
	pres = (struct crack_result *)pdata;

	CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Result password : %s,status %d,guid %s\n",pres->password,pres->status,pres->guid);

	ret = g_CrackBroker.GetWIResult(pres);
	if (ret < 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Result Error\n");
		

	}else {
		
		CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Result OK\n");
	}
	
	reshdr.dataLen = 0;
	reshdr.response = ret;
	
	memcpy(resBuf,&reshdr,sizeof(struct control_header));
	sendLen = sizeof(struct control_header);
	
	ret = doSendDataNew(pclient, resBuf, sendLen);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Get Workitem Result Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Result OK\n");
		ret = 0;
	}
	return ret;
}




static FUNC_MAP::value_type func_value_type[] ={

	//FUNC_MAP::value_type(TOKEN_HEARTBEAT,client_keeplive),
	//FUNC_MAP::value_type(TOKEN_LOGIN,client_login),

	FUNC_MAP::value_type(TOKEN_HEARTBEAT,client_keeplivenew),

	FUNC_MAP::value_type(TOKEN_LOGIN,client_loginnew),
	FUNC_MAP::value_type(CMD_GET_A_WORKITEM,comp_get_a_workitem_new),
	FUNC_MAP::value_type(CMD_WORKITEM_STATUS,comp_get_workitem_status_new),
	FUNC_MAP::value_type(CMD_WORKITEM_RESULT,comp_get_workitem_res_new),


	FUNC_MAP::value_type(CMD_TASK_UPLOAD,cc_task_uploadNew),

	FUNC_MAP::value_type(CMD_TASK_START,cc_task_startnew),
	FUNC_MAP::value_type(CMD_TASK_STOP,cc_task_stopnew),
	FUNC_MAP::value_type(CMD_TASK_DELETE,cc_task_deletenew),
	FUNC_MAP::value_type(CMD_TASK_PAUSE,cc_task_pausenew),
	FUNC_MAP::value_type(CMD_TASK_RESULT,cc_get_task_resultnew),
	FUNC_MAP::value_type(CMD_REFRESH_STATUS,cc_refresh_statusnew),
	FUNC_MAP::value_type(CMD_GET_CLIENT_LIST,cc_get_client_listnew),

	FUNC_MAP::value_type(CMD_START_DOWNLOAD,cc_task_file_download),
	FUNC_MAP::value_type(CMD_DOWNLOAD_FILE,cc_task_file_download_start),
	FUNC_MAP::value_type(CMD_END_DOWNLOAD,cc_task_file_download_end),

	
	FUNC_MAP::value_type(CMD_UPLOAD_FILE,cc_task_upload_file),
	FUNC_MAP::value_type(CMD_START_UPLOAD,cc_task_upload_file_start),



};

static FUNC_MAP recv_data_map(func_value_type,func_value_type+18);


INT doRecvData(LPVOID pclient, LPBYTE pdata, UINT len,BYTE cmdflag){
	if(recv_data_map.find(cmdflag) == recv_data_map.end())
		return -1;

	return (*recv_data_map[cmdflag])(pclient,pdata,len);
}