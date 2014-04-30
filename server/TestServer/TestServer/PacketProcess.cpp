#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>

#include "PacketProcess.h"
#include "zlib.h"
#include "ServerResp.h"
#include "guidgenerator.h"
#include "macros.h"
#include "algorithm_types.h"

#include "ReqPacket.h"
#include "ResPacket.h"
#include "CrackBroker.h"
#include "CLog.h"

#pragma comment(lib,"Shlwapi.lib")


#define FILE_DIR ".\\tempdir\\"

//#define FILE_DIR ".\\"

CCrackBroker g_CrackBroker;

int getClientIPInfo(void *pclient,char *pip,int *port){
	
	SOCKET clisock = *(SOCKET *)pclient;

	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	getpeername(clisock, (sockaddr *)&addr, &addrlen);

	strcpy(pip, inet_ntoa(addr.sin_addr));
	*port = ntohs(addr.sin_port);

	return 0;
}

int client_login(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	char buf[40];
	char ip[20];
	int port = 0;

	client_login_req *pC = (client_login_req *)pdata;
	client_login_req myclient;

	getClientIPInfo(pclient,ip,&port);

	if (pdata == NULL){
		
		memset(&myclient,0,sizeof(client_login_req));
		memset(buf,0,40);
		sprintf(buf,"%u",*(SOCKET *)pclient);
		myclient.m_clientsock = *(SOCKET *)pclient;
		myclient.m_cputhreads = myclient.m_gputhreads = 0;
		myclient.m_type = 0;
		memcpy(myclient.m_ip,ip,16);

		pC = &myclient;
		
	}

	pC->m_port = port;
	pC->m_clientsock = *(SOCKET *)pclient;
	memcpy(pC->m_ip,ip,strlen(ip));
	//����ҵ���߼�

	nRet = g_CrackBroker.ClientLogin(pC);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Client Login IP %s port %d Error\n",ip,port);
	}else {
		//CLog::Log(LOG_LEVEL_WARNING,"Client Login IP %s port %d OK\n",ip,port);
	}
	////////////////////////////////////

	//����Ӧ���ģ�������

	int m = Write(*(SOCKET*)pclient, CMD_LOGIN, 0, NULL,0,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Login :Send Response Error %d \n",ip,port,m);
		
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Login OK\n",ip,port);
	}

	return nRet;
}

//ͬ��������Ӧ
int client_keeplive(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	//CLog::Log(LOG_LEVEL_WARNING,"This is a client keeplive\n");
	//struct client_keeplive_req *pKeeplive = (struct client_keeplive_req *)pdata;
	//struct client_keeplive_req keeplive;
	char ip[20];
	int port = 0;
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);

#if 0		
	nRet = g_CrackBroker.ClientKeepLive(ip);

	//����Ӧ�𲢷���
	int m = Write(*(SOCKET*)pclient, COMMAND_REPLAY_HEARTBEAT, 0, NULL,0,true);
#else
	unsigned char cmd;
	char* data = NULL;
	nRet = g_CrackBroker.ClientKeepLive2(ip, pclient, &cmd, (void **)&data);
	int m = Write(*(SOCKET*)pclient, cmd, 0, data, nRet, true);
	g_CrackBroker.Free(data);
#endif
	
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Keeplive :Send Response Error %d \n",ip,port,m);
	}else	{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client KeepLive %d OK\n",ip, port, cmd);
	}

	return nRet;
}


int cc_task_upload(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task upload New\n");
	int nRet = 0;
	int m = 0;
	unsigned int resLen = 0;

	crack_task *pCrackTask = NULL;
	task_upload_res task_upload;
	char ip[20];
	int port = 0;
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);


	if (len != sizeof(crack_task)){

		CLog::Log(LOG_LEVEL_WARNING,"Upload Task: Data len no crack_task size Error\n");
		return -2;

	}

	pCrackTask = (crack_task *)pdata;
	if(pCrackTask->single == 0)
		CLog::Log(LOG_LEVEL_NOMAL,"Incoming a new Task [charset=%d, filename=%s, algo=%d]\n",pCrackTask->charset,pCrackTask->filename,pCrackTask->algo);
	else
		CLog::Log(LOG_LEVEL_NOMAL,"Incoming a new Task [charset=%d, hash_value=%s, algo=%d]\n",pCrackTask->charset,pCrackTask->filename,pCrackTask->algo);
	
	new_guid(pCrackTask->guid,sizeof(pCrackTask->guid));
	
	nRet = g_CrackBroker.CreateTask(pCrackTask,task_upload.guid);
	if (nRet < 0) {
		resLen = 0;
	}else{
		resLen = sizeof(struct task_upload_res);
	}

	if(pCrackTask->single)
	{
		nRet = g_CrackBroker.SplitTask((char *)task_upload.guid, (char *)pCrackTask->filename);
		if (nRet < 0){
			CLog::Log(LOG_LEVEL_WARNING, "Broker Split ErrorCode:%d\n", nRet);
			
			//ɾ���´�������
			g_CrackBroker.deleteTask((char *)task_upload.guid);
		}
	}
	
	memcpy(task_upload.guid,pCrackTask->guid,40);

	m = Write(*(SOCKET*)pclient, CMD_TASK_UPLOAD, nRet, &task_upload,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Upload Task: Send Response Error %d \n",ip,port,m);
		
	}else	
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Upload Task OK\n",ip,port);

	return nRet;
}


int cc_task_start(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_NOMAL, "this is task start new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	task_start_req *pStartReq = NULL;
	task_status_res taskres;
	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);

	if (len != sizeof(task_start_req)){
		CLog::Log(LOG_LEVEL_WARNING,"Start Task:Data len not task_start_req size Error\n");
		return -2;
	}

	pStartReq = (task_start_req *)pdata;
	CLog::Log(LOG_LEVEL_NOMAL, "Req Start Task guid:%s\n",pStartReq->guid);
		
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pStartReq->guid,strlen((char *)pStartReq->guid));

	//ҵ����
	nRet = g_CrackBroker.StartTask(pStartReq);
	if (nRet < 0 ){
		resLen = 0;
	}else{
		resLen = sizeof(task_status_res);
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_TASK_START, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Start Task :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Start Task OK\n",ip,port);

	return nRet;
}

//stop task

int cc_task_stop(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_NOMAL, "this is task Stop new\n");
	int nRet = 0;
	unsigned int resLen = 0;

	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_STOP);
	struct task_stop_req *pStopReq = NULL;
	struct task_status_res taskres;
	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);

	if (len != sizeof(task_stop_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task : Data len not task_stop_req size Error\n");
		return -2;

	}
	pStopReq = (struct task_stop_req *)pdata;
	CLog::Log(LOG_LEVEL_NOMAL,"Stop Task guid:%s\n",pStopReq->guid);
		
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pStopReq->guid,strlen((char *)pStopReq->guid));

	//ҵ����
	nRet = g_CrackBroker.StopTask(pStopReq);
	if (nRet < 0 ){
		resLen = 0;
	}else{
		resLen = sizeof(task_status_res);
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_TASK_STOP, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Stop Task :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Stop Task OK\n",ip,port);

	return nRet;
}


//pause task

int cc_task_pause(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_NOMAL,"this is task pause new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	task_pause_req *pPauseReq = NULL;
	task_status_res taskres;

	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);

	if (len != sizeof(task_pause_req)){
		CLog::Log(LOG_LEVEL_WARNING,"Pause Task : Data len not task_pause_req size Error\n");
		return -2;
	}

	pPauseReq = (task_pause_req *)pdata;

	CLog::Log(LOG_LEVEL_NOMAL,"Pause Task guid:%s\n",pPauseReq->guid);
		
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pPauseReq->guid,strlen((char *)pPauseReq->guid));

	//����ҵ���߼�
	nRet = g_CrackBroker.PauseTask(pPauseReq);
	if (nRet < 0){
		resLen = 0;
	}else{
		resLen = sizeof(struct task_status_res);
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_TASK_PAUSE, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Pause Task :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Pause Task OK\n",ip,port);

	return nRet;
}

//delete task

int cc_task_delete(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_NOMAL,"this is task Delete new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	task_delete_req *pDeleteReq = NULL;
	task_status_res taskres;

	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);

	memset(&taskres,0,sizeof(struct task_status_res));
	if (len != sizeof(task_delete_req)){

		CLog::Log(LOG_LEVEL_WARNING,"Delete Task : Data len not task_delete_req size Error\n");
		return -2;
	}

	pDeleteReq = (task_delete_req *)pdata;

	CLog::Log(LOG_LEVEL_NOMAL,"Delete Task guid: %s\n",pDeleteReq->guid);

	memcpy(taskres.guid,pDeleteReq->guid,strlen((char *)pDeleteReq->guid));

	//����ҵ���߼�
	nRet = g_CrackBroker.DeleteTask(pDeleteReq);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Delete Task %s ErrorCode: %d\n",pDeleteReq->guid,nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Broker Delete Task %s OK\n",pDeleteReq->guid);
		resLen = sizeof(struct task_status_res);
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_TASK_DELETE, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Delete Task :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Delete Task OK\n",ip,port);

	return nRet;
}


//get a task result,��̬���ٿռ�
int cc_get_task_result(void *pclient, unsigned char * pdata, UINT len){

	int nRet = 0;
	unsigned int resLen = 0;
	task_result_req *pResReq = NULL;
	task_result_info *pres = NULL;
	int resNum = 0;

	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);
	if (len != sizeof(task_result_req)){
		CLog::Log(LOG_LEVEL_WARNING,"Get A Task Result: Data len not task_result_req size Error\n");
		return -2;
	}

	pResReq = (task_result_req *)pdata;

	CLog::Log(LOG_LEVEL_NOMAL,"Get a Task result guid: %s\n",pResReq->guid);
	
	//����ҵ���߼�
	nRet = g_CrackBroker.GetTaskResult(pResReq,&pres,&resNum);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task %s Result, ErrorCode : %d\n",pResReq->guid,nRet);
		resLen = 0;
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task %s Result OK\n",pResReq->guid);
		resLen = sizeof(struct task_result_info)*resNum;
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_TASK_RESULT, nRet, pres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task %s Result: Send Response Error %d \n",ip,port,pResReq->guid,m);
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Get Task %s Result OK\n",ip,port,pResReq->guid);
	}

	g_CrackBroker.Free(pres);

	return nRet;
}


//get task status����̬���ٿռ�
int cc_refresh_status(void *pclient, unsigned char * pdata, UINT len){

	int nRet = 0;
	unsigned int resLen = 0;
	struct task_status_info *pTasksStatus = NULL;
	unsigned int resNum = 0;

	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);
		
	//����ҵ���߼�
	nRet = g_CrackBroker.GetTasksStatus(&pTasksStatus,&resNum);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task Status %d Result ,ErrorCode : %d\n",resNum,nRet);
		resLen = 0;
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"Broker Get Task Status %d Result OK\n",resNum);
		resLen = sizeof(struct task_status_info) * resNum;
	}
	
	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_REFRESH_STATUS, nRet, pTasksStatus,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task Status :Send Response Error %d \n",ip,port,m);
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Get Task Status OK\n",ip,port);
	}

	g_CrackBroker.Free(pTasksStatus);
	return nRet;
}


//get client list, ��̬���ٿռ�
int cc_get_client_list(void *pclient, unsigned char * pdata, UINT len){

	int nRet = 0;
	unsigned int resLen = 0;
	struct compute_node_info *pClients = NULL;
	unsigned int resNum = 0;

	char ip[20];
	int port = 0;

	memset(ip,0,20);

	getClientIPInfo(pclient,ip,&port);
	//����ҵ���߼�
	nRet = g_CrackBroker.GetClientList(&pClients,&resNum);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Broker Get Clients ,number is %d ,ErrorCode : %d\n",resNum,nRet);
	
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Broker Get Clients, number is %d OK\n",resNum);
		resLen = sizeof(struct compute_node_info) * resNum;
	}
	
	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_GET_CLIENT_LIST, nRet, pClients,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Client Info: Send Response Error %d \n",ip,port,m);
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Get Client Info OK\n",ip,port);
	}

	g_CrackBroker.Free(pClients);
	return nRet;
}

//download file res
// download file 
int cc_task_file_download(void *pclient, unsigned char * pdata,UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is file download start \n");
	int ret = 0;
	FILE *pfile = NULL;
	file_info fileinfo;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_DOWNLOAD_FILE);
	download_file_req *preq = NULL;
	unsigned int filelen =  0;
	int resLen = 0;

	char ip[20];
	int port = 0;
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);

	preq = (struct download_file_req *)pdata;
	char task_guid[40];
	if(g_CrackBroker.QueryTaskByWI(task_guid, (char *)preq->guid) != 0)
	{
		ret = -1;
		goto resp;
	}

	char filename[MAX_PATH];
	sprintf(filename,"%s%s",FILE_DIR, task_guid);
	pfile = fopen(filename,"rb");
	if (!pfile){
		
		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s error %d\n", filename, GetLastError());
		
		ret = -1;

	}else {
		fseek(pfile,0L,SEEK_END);
		filelen = ftell(pfile);  //��ȡ�ļ�����
		fseek(pfile,0L,SEEK_SET);
				
		fileinfo.f = pfile;
		fileinfo.len = filelen;
		fileinfo.offset = 0;

		CLog::Log(LOG_LEVEL_NOMAL,"pfile %p, len:%d, offset:%d, guid:%s\n",fileinfo.f,fileinfo.len,fileinfo.offset,preq->guid);

		resLen = sizeof(struct file_info);
	}

resp:
	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_DOWNLOAD_FILE, ret, &fileinfo,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Downnload :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Get Task File Download OK\n",ip,port);
	
	return ret;
}

//download file res tran
//download file start
int cc_task_file_download_start(void *pclient,unsigned char *pdata,UINT len){
	
	CLog::Log(LOG_LEVEL_NOMAL,"this is file download ...\n");
	int ret = 0;
	int rdlen = 0;
	int readLen = 0;
	FILE *pfile = NULL;
	SOCKET sock = *(SOCKET *)pclient;
	BYTE resBuf[MAX_BUF_LEN];
	file_info *pFileInfo = NULL;

	char ip[20];
	int port = 0;
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);

	pFileInfo = (file_info *)pdata;
	pfile = (FILE *)pFileInfo->f;

	CLog::Log(LOG_LEVEL_NOMAL,"pfile %p, len:%d, offset:%d\n",pFileInfo->f,pFileInfo->len,pFileInfo->offset);

	if (pFileInfo->len > 8196){		
		rdlen = 8196;
	}else{
		rdlen = pFileInfo->len;
	}

	while(!feof(pfile)){

		memset(resBuf,0,MAX_BUF_LEN);
		readLen  = fread(resBuf,1,rdlen,pfile);

		//CLog::Log(LOG_LEVEL_WARNING,"file read %d %d\n",readLen,rdlen);

		if (readLen < 0){

			CLog::Log(LOG_LEVEL_WARNING,"file read error.\n");
			fclose(pfile);
			ret = -2;
			readLen = 0;
		}else if(readLen == 0){

			fclose(pfile);
			ret = 0;

		}else {
			ret = 0;
			ret = Write(sock, CMD_START_DOWNLOAD, ret,resBuf,readLen,false);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"file download data block Error\n");
				break;

			}else{
				//CLog::Log(LOG_LEVEL_WARNING,"file download data block .len : %d OK\n",readLen);
				readLen = 0;
			}
		}		
	}

	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Download ... :Send Response Error %d \n",ip,port,ret);

	}else	CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Task File Download ...OK\n",ip,port);

	return ret;
}


//download file res end
//download file end
int cc_task_file_download_end(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_NOMAL,"this is file download end\n");
	int ret = 0;
	unsigned int resLen = 0;
	file_info *pFileInfo = NULL;
	file_info finfo;
	FILE *pfile = NULL;
	char ip[20];
	int port = 0;
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);
	
	pFileInfo = (file_info *)pdata;
	pfile = (FILE *)pFileInfo->f;
	fclose(pfile);

	resLen = sizeof(struct file_info);
	
	memset(&finfo,0,sizeof(file_info));

	finfo.f = pfile;
	finfo.len = pFileInfo->len;
	finfo.offset = pFileInfo->offset;


	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_END_DOWNLOAD, 0, &finfo,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Downnload End :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Get Task File Download End OK\n",ip,port);

	return ret;
}

//control client : upload start

//upload file req  
int cc_task_upload_file(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_NOMAL,"this is file upload\n");
	int ret = 0;
	unsigned int resLen = 0;
	FILE *pfile = NULL;

	file_upload_res uploadres;
	file_upload_req *preq = NULL;
	char ip[20];
	int port = 0;
	char filename[128];
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);

	preq = (struct file_upload_req *)pdata;

	memset(&uploadres,0,sizeof(struct file_upload_res));
	memcpy(uploadres.guid,preq->guid,40);

	memset(filename,0,128);
	sprintf((char *)filename,"%s%s",FILE_DIR,(char *)uploadres.guid);


	int retval = PathFileExistsA(FILE_DIR);
	if (retval == 1){
		CLog::Log(LOG_LEVEL_WARNING,"DIR %s Exists\n",FILE_DIR);

	}else{
		retval = CreateDirectoryA(FILE_DIR,NULL);
		if (retval == 0){
			CLog::Log(LOG_LEVEL_WARNING,"DIR %s Create Eroor\n",FILE_DIR);
		}else{
			CLog::Log(LOG_LEVEL_NOMAL,"DIR %s Create OK\n",FILE_DIR);
		}
	}

	pfile = fopen((char *)filename,"wb");
	if (!pfile){
		CLog::Log(LOG_LEVEL_WARNING,"File %s open error\n",filename);	
		ret = -1;
		uploadres.f = NULL;

	}else{
		uploadres.f = pfile;
		ret = 0;
	}

	uploadres.len = 0;
	uploadres.offset = 0;

	resLen = sizeof(struct file_upload_res);

	CLog::Log(LOG_LEVEL_NOMAL,"Upload File: pfile %p, len:%d ,offset:%d, guid:%s\n",uploadres.f,uploadres.len,uploadres.offset,uploadres.guid);

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_UPLOAD_FILE, ret, &uploadres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Upload :Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Get Task File Upload OK\n",ip,port);

	return ret;
}

//upload file start req  
int cc_task_upload_file_start(void *pclient,unsigned char *pdata,UINT len){
	
	CLog::Log(LOG_LEVEL_NOMAL,"this is file upload .... \n");
	int ret = 0;
	FILE *pfile = NULL;

	SOCKET sock = *(SOCKET *)pclient;
	file_upload_start_req *preq = NULL;
	unsigned int filelen =  0;
	unsigned int readLen = 0;
	unsigned int writeFileLen = 0;
	unsigned char cmd = 0;
	short status = 0;
	unsigned char filename[128];
	unsigned char resBuf[MAX_BUF_LEN];
	char tmpguid[40];
	
	char ip[20];
	int port = 0;
	time_t tmpTime = 0;
	getClientIPInfo(pclient,ip,&port);

	preq = (file_upload_start_req *)pdata;
	filelen = preq->len;
	pfile = (FILE *)preq->f;
	CLog::Log(LOG_LEVEL_NOMAL,"upload start guid:%s  file:%p, filelen:%d,\n",preq->guid,preq->f,preq->len);
	
	if (!pfile){
		CLog::Log(LOG_LEVEL_WARNING,"file is null\n");
		return -1;
	}

	memset(tmpguid,0,40);
	memcpy(tmpguid,preq->guid,40);

	CLog::Log(LOG_LEVEL_NOMAL,"upload start guid:%s\n",preq->guid);
	memset(filename,0,128);
	sprintf((char *)filename,"%s%s",".\\tempdir\\",(char *)tmpguid);

	CLog::Log(LOG_LEVEL_NOMAL,"filename: %s \n",filename);
	CLog::Log(LOG_LEVEL_NOMAL,"pfile %p, len:%d, offset:%d, guid:%s\n",preq->f,preq->len,preq->offset,preq->guid);

	unsigned int curlen = 0;
	
	while(curlen < filelen ){
		
		memset(resBuf,0,MAX_BUF_LEN);
		readLen = Read(sock,&cmd,&status,resBuf,MAX_BUF_LEN);
		if (readLen < 0 ){
			
			CLog::Log(LOG_LEVEL_WARNING,"Recv client upload file %s ...\n",filename);
			ret = -2;
			break;

		}else{

			curlen += readLen;
			//recv the file data 
			writeFileLen = fwrite(resBuf,1,readLen,pfile);
			if (writeFileLen < 1){
				CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Upload writelen %d  Error %d \n",ip,port,writeFileLen,readLen);
				ret = -3;
				break;

			}else{	
				//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Upload writelen %d OK\n",ip,port,writeFileLen);					
			}
		}
	}

	return 0;
}


//��Ӵ����ϴ���������

int cc_task_upload_file_end(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_NOMAL,"this is file upload End \n");
	int ret = 0;
	int nRet = 0;
	FILE *pfile = NULL;
	unsigned int resLen = 0;
	file_upload_end_req *preq = NULL;
	file_upload_end_res uploadres;
	control_header reshdr = INITIALIZE_EMPTY_HEADER(CMD_END_UPLOAD);
	unsigned int filelen =  0;
	char ip[20];
	int port = 0;
	time_t tmpTime = 0;

	getClientIPInfo(pclient,ip,&port);
	preq = (struct file_upload_end_req *)pdata;
	memset(&uploadres,0,sizeof(struct file_upload_end_res));

	pfile = (FILE *)preq->f;

	if (!pfile){	
		CLog::Log(LOG_LEVEL_WARNING,"file upload end req guid %s, file is NULL\n",preq->guid);
		ret = -1;
		uploadres.f = NULL;
		resLen = 0;
	}else {
		ret = 0;
		fclose(pfile);
		uploadres.f = pfile;
		pfile = NULL;
		resLen = sizeof(struct file_upload_end_res);

		CLog::Log(LOG_LEVEL_NOMAL,"file upload end  req guid %s, Close File %p\n",preq->guid,uploadres.f);
	}

	memcpy(uploadres.guid,preq->guid,40);
	uploadres.len = preq->len;
	uploadres.offset = preq->offset;

	//�޸����̣������з֣���������ڷ����зִ���ԭ��
	if (ret == 0){	
		//ͨ������guid ���в��ң����ҷ�����

		//����ҵ���߼�
		//ret == -300 load file error 
		//ret == -301 split hash error
		ret = g_CrackBroker.SplitTask((char *)uploadres.guid);
		if (ret < 0){
			CLog::Log(LOG_LEVEL_WARNING,"Broker Split ErrorCode : %d\n",ret);
			
			//ɾ���´�������
			nRet = g_CrackBroker.deleteTask((char *)uploadres.guid);
			if (nRet < 0 ){
				CLog::Log(LOG_LEVEL_WARNING,"Delete Task %s, Error:%d, Split ErrorCode:%d\n",(char *)uploadres.guid,nRet,ret);
				ret = -100;  //ɾ���������
			}
		}else{
			CLog::Log(LOG_LEVEL_NOMAL, "SplitTask OK\n",ret);
		}
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_END_UPLOAD, ret, &uploadres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get Task File Upload End:Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"[%s:%d] Client Get Task File Upload End OK\n",ip,port);

	return ret;
}

//����һ��������
int comp_get_a_workitem(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_block *pcrackblock = NULL;
	unsigned int resLen = 0;
	char ip[20];
	int port = 0;

	memset(ip,0,20);
	getClientIPInfo(pclient,ip,&port);

	//����ҵ���߼�
	//ret = g_CrackBroker.GetAWorkItem(&pcrackblock);
	ret = g_CrackBroker.GetAWorkItem2(ip,&pcrackblock);
	if (ret < 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get WorkItem Error %d\n", ip, port, ret);
		resLen = 0;	
	}else {
		CLog::Log(LOG_LEVEL_NOMAL, "[%s:%d] Get WorkItem OK [guid=%s, algo=%d, type=%d]\n", 
			ip, port, pcrackblock->guid, pcrackblock->algo, pcrackblock->type);
		resLen = sizeof(struct crack_block);
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_GET_A_WORKITEM, ret, pcrackblock,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get WorkItem:Send Response Error %d \n",ip,port,m);

	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Get A WorkItem OK\n",ip,port);
	}
	g_CrackBroker.Free(pcrackblock);

	return ret;
}

//��������״̬
int comp_get_workitem_status(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_status *pstatus = NULL;
	char ip[20];
	int port = 0;

	memset(ip,0,20);
	getClientIPInfo(pclient,ip,&port);
	pstatus = (struct crack_status *)pdata;

	ret = g_CrackBroker.GetWIStatus(pstatus);
	if (ret < 0 ){
		//CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Progress Error %d\n", ret);
	}else {
		//CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Progress OK\n");
	}
	
	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_WORKITEM_STATUS, ret, NULL,0,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get A WorkItem %s Progress :Send Response Error %d \n",ip,port,pstatus->guid,m);
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Get A WorkItem %s Progress OK\n",ip,port,pstatus->guid);
	}
	return ret;
}


//�������ִ�н��
int comp_get_workitem_res(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_result *pres = NULL;

	char ip[20];
	int port = 0;

	memset(ip,0,20);
	getClientIPInfo(pclient,ip,&port);

	pres = (struct crack_result *)pdata;
	ret = g_CrackBroker.GetWIResult(pres);
	if (ret < 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get A WorkItem Result Error %d\n", ip,port, ret);
	}else {
		//CLog::Log(LOG_LEVEL_WARNING,"Get A WorkItem Result OK\n");
	}

	//����Ӧ���ģ�������
	int m = Write(*(SOCKET*)pclient, CMD_WORKITEM_RESULT, ret, NULL,0,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Get A WorkItem %s Result: Send Response Error %d \n",ip,port,pres->guid,m);
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"[%s:%d] Client Get A WorkItem %s Result OK\n",ip,port,pres->guid);
	}
	return ret;
}

int client_quit(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;

	char ip[20];
	int port = 0;

	memset(ip,0,20);
	memcpy(ip,(char *)pdata,strlen((char *)pdata));
	port = len;

	ret = g_CrackBroker.DoClientQuit(ip,port);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Can't find the Client %s:%d\n",ip,port);

		ret = -1;
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Client Quit [%s:%d] OK\n",ip,port);
	}
	return ret;
}

//����ӳ����ʼ��
static FUNC_MAP::value_type func_value_type[] ={

	FUNC_MAP::value_type(CMD_HEARTBEAT,client_keeplive),

	FUNC_MAP::value_type(CMD_LOGIN,client_login),
	FUNC_MAP::value_type(CMD_GET_A_WORKITEM,comp_get_a_workitem),
	FUNC_MAP::value_type(CMD_WORKITEM_STATUS,comp_get_workitem_status),
	FUNC_MAP::value_type(CMD_WORKITEM_RESULT,comp_get_workitem_res),


	FUNC_MAP::value_type(CMD_TASK_UPLOAD,cc_task_upload),

	FUNC_MAP::value_type(CMD_TASK_START,cc_task_start),
	FUNC_MAP::value_type(CMD_TASK_STOP,cc_task_stop),
	FUNC_MAP::value_type(CMD_TASK_DELETE,cc_task_delete),
	FUNC_MAP::value_type(CMD_TASK_PAUSE,cc_task_pause),
	FUNC_MAP::value_type(CMD_TASK_RESULT,cc_get_task_result),
	FUNC_MAP::value_type(CMD_REFRESH_STATUS,cc_refresh_status),
	FUNC_MAP::value_type(CMD_GET_CLIENT_LIST,cc_get_client_list),

	
	FUNC_MAP::value_type(CMD_DOWNLOAD_FILE,cc_task_file_download),  //���������
	FUNC_MAP::value_type(CMD_START_DOWNLOAD,cc_task_file_download_start),		 //��ʼ����
	FUNC_MAP::value_type(CMD_END_DOWNLOAD,cc_task_file_download_end),	 //���ؽ��� 

	
	FUNC_MAP::value_type(CMD_UPLOAD_FILE,cc_task_upload_file),			//�ϴ������
	FUNC_MAP::value_type(CMD_START_UPLOAD,cc_task_upload_file_start),	//��ʼ�ϴ�
	FUNC_MAP::value_type(CMD_END_UPLOAD,cc_task_upload_file_end),		//�ϴ�����

	FUNC_MAP::value_type(CMD_CLIENT_QUIT,client_quit),		//����ͻ����˳�

};

static FUNC_MAP recv_data_map(func_value_type,func_value_type+sizeof(func_value_type)/sizeof(func_value_type[0]));


INT doRecvData(void* pclient, unsigned char* pdata, UINT len,BYTE cmdflag){
	if(recv_data_map.find(cmdflag) == recv_data_map.end())
		return -1;

	return (*recv_data_map[cmdflag])(pclient,pdata,len);
}