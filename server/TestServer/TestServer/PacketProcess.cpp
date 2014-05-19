#include <windows.h>
#include <stdio.h>

#include "macros.h"
#include "err.h"
#include "algorithm_types.h"
#include "CLog.h"
#include "ReqPacket.h"
#include "ResPacket.h"

#include "PacketProcess.h"
#include "ServerResp.h"
#include "guidgenerator.h"
#include "ClientInfo.h"
#include "CrackBroker.h"

std::map<void* , void*> open_files;

CCrackBroker g_CrackBroker;

//同步心跳回应
int client_keeplive(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	char* owner = client->GetOwner();
	SOCKET cliSocket = client->GetSocket();

	unsigned char cmd;
	char* data = NULL;
	nRet = g_CrackBroker.ClientKeepLive2(owner, (void *)cliSocket, &cmd, (void **)&data);
	int m = Write(cliSocket, cmd, 0, data, nRet, true);
	g_CrackBroker.Free(data);
	
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"[%s:%d] Keeplive :Send Response Error %d \n",ip,port,m);
	}else	{
		CLog::Log(LOG_LEVEL_DEBUG,"[%s:%d] Client KeepLive %d OK\n",ip, port, cmd);
	}

	return nRet;
}


int cc_task_upload(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_DEBUG,"cc_task_upload: this is task upload New\n");
	int nRet = 0;
	int m = 0;
	unsigned int resLen = 0;

	crack_task *pCrackTask = NULL;
	task_upload_res task_upload;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	if (len != sizeof(crack_task)){

		CLog::Log(LOG_LEVEL_WARNING,"cc_task_upload: Data len no crack_task size Error\n");
		return ERR_INVALIDDATA;

	}

	pCrackTask = (crack_task *)pdata;
	if(pCrackTask->single == 0)
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_upload: Uploading [charset=%d, filename=%s, algo=%d]\n",pCrackTask->charset,pCrackTask->filename,pCrackTask->algo);
	else
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_upload: Uploading [charset=%d, hash_value=%s, algo=%d]\n",pCrackTask->charset,pCrackTask->filename,pCrackTask->algo);
	
	new_guid(pCrackTask->guid, sizeof(pCrackTask->guid));
	memcpy(task_upload.guid, pCrackTask->guid, sizeof(task_upload.guid));
	
	nRet = g_CrackBroker.CreateTask(pCrackTask, pclient);
	if (nRet < 0) {
		resLen = 0;
	}else{
		resLen = sizeof(struct task_upload_res);
	}

	if(pCrackTask->single)
	{
		nRet = g_CrackBroker.SplitTask((char *)task_upload.guid, (char *)pCrackTask->filename);
		if (nRet < 0){
			CLog::Log(LOG_LEVEL_WARNING, "cc_task_upload: Broker Split ErrorCode:%d\n", nRet);
			
			//删除新创建任务
			g_CrackBroker.deleteTask((char *)task_upload.guid, pclient);
		}
	}
	
	memcpy(task_upload.guid,pCrackTask->guid,40);

	m = Write(cliSocket, CMD_TASK_UPLOAD, nRet, &task_upload,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"cc_task_upload: [%s:%d] Send Response Error %d\n",ip,port,m);
		
	}else	
		CLog::Log(LOG_LEVEL_NOTICE,"cc_task_upload: [%s:%d] Upload Task OK GUID=%s\n",ip,port,task_upload.guid);

	return nRet;
}


int cc_task_start(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_DEBUG, "cc_task_start: this is task start new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	task_start_req *pStartReq = NULL;
	task_status_res taskres;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	if (len != sizeof(task_start_req)){
		CLog::Log(LOG_LEVEL_DEBUG,"cc_task_start: Data len not task_start_req size Error\n");
		return ERR_INVALIDDATA;
	}

	pStartReq = (task_start_req *)pdata;
	CLog::Log(LOG_LEVEL_NOMAL, "cc_task_start: Task guid %s\n",pStartReq->guid);
		
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pStartReq->guid,strlen((char *)pStartReq->guid));

	//业务处理
	nRet = g_CrackBroker.StartTask(pStartReq, pclient);
	if (nRet < 0 ){
		resLen = 0;
	}else{
		resLen = sizeof(task_status_res);
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_TASK_START, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"cc_task_start: [%s:%d] Send Response Error %d\n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_start: [%s:%d] Client Start Task OK\n",ip,port);

	return nRet;
}

//stop task

int cc_task_stop(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_DEBUG, "cc_task_stop: this is task Stop new\n");
	int nRet = 0;
	unsigned int resLen = 0;

	struct task_stop_req *pStopReq = NULL;
	struct task_status_res taskres;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	if (len != sizeof(task_stop_req)){

		CLog::Log(LOG_LEVEL_WARNING,"cc_task_stop: Data len not task_stop_req size Error\n");
		return ERR_INVALIDDATA;

	}
	pStopReq = (struct task_stop_req *)pdata;
	CLog::Log(LOG_LEVEL_NOMAL,"cc_task_stop: Task guid %s\n",pStopReq->guid);
		
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pStopReq->guid,strlen((char *)pStopReq->guid));

	//业务处理
	nRet = g_CrackBroker.StopTask(pStopReq, pclient);
	if (nRet < 0 ){
		resLen = 0;
	}else{
		resLen = sizeof(task_status_res);
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_TASK_STOP, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"cc_task_stop: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_stop: [%s:%d] Client Stop Task OK\n",ip,port);

	return nRet;
}


//pause task

int cc_task_pause(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_DEBUG,"cc_task_pause: this is task pause new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	task_pause_req *pPauseReq = NULL;
	task_status_res taskres;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	if (len != sizeof(task_pause_req)){
		CLog::Log(LOG_LEVEL_WARNING,"cc_task_pause: Data len not task_pause_req size Error\n");
		return ERR_INVALIDDATA;
	}

	pPauseReq = (task_pause_req *)pdata;

	CLog::Log(LOG_LEVEL_NOMAL,"cc_task_pause: Task guid %s\n",pPauseReq->guid);
		
	memset(&taskres,0,sizeof(struct task_status_res));
	memcpy(taskres.guid,pPauseReq->guid,strlen((char *)pPauseReq->guid));

	//处理业务逻辑
	nRet = g_CrackBroker.PauseTask(pPauseReq, pclient);
	if (nRet < 0){
		resLen = 0;
	}else{
		resLen = sizeof(struct task_status_res);
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_TASK_PAUSE, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"cc_task_pause: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_pause: [%s:%d] Client Pause Task OK\n",ip,port);

	return nRet;
}

//delete task

int cc_task_delete(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_DEBUG,"cc_task_delete: this is task Delete new\n");
	
	int nRet = 0;
	unsigned int resLen = 0;
	task_delete_req *pDeleteReq = NULL;
	task_status_res taskres;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	memset(&taskres,0,sizeof(struct task_status_res));
	if (len != sizeof(task_delete_req)){

		CLog::Log(LOG_LEVEL_WARNING,"cc_task_delete: Data len not task_delete_req size Error\n");
		return ERR_INVALIDDATA;
	}

	pDeleteReq = (task_delete_req *)pdata;

	CLog::Log(LOG_LEVEL_NOMAL,"cc_task_delete: Task guid %s\n",pDeleteReq->guid);

	memcpy(taskres.guid,pDeleteReq->guid,strlen((char *)pDeleteReq->guid));

	//处理业务逻辑
	nRet = g_CrackBroker.DeleteTask(pDeleteReq, pclient);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"cc_task_delete: Broker Delete %s ErrorCode %d\n",pDeleteReq->guid,nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_delete: Broker Delete Task %s OK\n",pDeleteReq->guid);
		resLen = sizeof(struct task_status_res);
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_TASK_DELETE, nRet, &taskres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"cc_task_delete: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"cc_task_delete: [%s:%d] Client Delete Task OK\n",ip,port);

	return nRet;
}


//get a task result,动态开辟空间
int cc_get_task_result(void *pclient, unsigned char * pdata, UINT len){

	int nRet = 0;
	unsigned int resLen = 0;
	task_result_req *pResReq = NULL;
	task_result_info *pres = NULL;
	int resNum = 0;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	if (len != sizeof(task_result_req)){
		CLog::Log(LOG_LEVEL_WARNING,"cc_get_task_result: Data len not task_result_req size Error\n");
		return ERR_INVALIDDATA;
	}

	pResReq = (task_result_req *)pdata;

	CLog::Log(LOG_LEVEL_DEBUG, "cc_get_task_result: Task guid %s\n",pResReq->guid);
	
	//处理业务逻辑
	nRet = g_CrackBroker.GetTaskResult(pResReq,&pres,&resNum, pclient);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"cc_get_task_result: Broker Get Task %s ErrorCode %d\n",pResReq->guid,nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_DEBUG,"Broker Get Task %s Result OK\n",pResReq->guid);
		resLen = sizeof(struct task_result_info)*resNum;
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_TASK_RESULT, nRet, pres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"cc_get_task_result: [%s:%d] Send Response Error %d \n",ip,port,pResReq->guid,m);
	}else{
		CLog::Log(LOG_LEVEL_DEBUG,"cc_get_task_result: [%s:%d] Client Get Task %s Result OK\n",ip,port,pResReq->guid);
	}

	g_CrackBroker.Free(pres);

	return nRet;
}


//get task status，动态开辟空间
int cc_refresh_status(void *pclient, unsigned char * pdata, UINT len){

	int nRet = 0;
	unsigned int resLen = 0;
	struct task_status_info *pTasksStatus = NULL;
	unsigned int resNum = 0;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();
		
	//处理业务逻辑
	nRet = g_CrackBroker.GetTasksStatus(&pTasksStatus,&resNum, pclient);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_DEBUG, "cc_refresh_status: Broker Get Task ErrorCode %d\n",nRet);
		resLen = 0;
	}else{
		CLog::Log(LOG_LEVEL_DEBUG,"cc_refresh_status: Broker Get Task Status %d Result OK\n",resNum);
		resLen = sizeof(struct task_status_info) * resNum;
	}
	
	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_REFRESH_STATUS, nRet, pTasksStatus,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"cc_refresh_status: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else{
		CLog::Log(LOG_LEVEL_DEBUG,"cc_refresh_status: [%s:%d] Client Get Task Status OK\n",ip,port);
	}

	g_CrackBroker.Free(pTasksStatus);
	return nRet;
}


//get client list, 动态开辟空间
int cc_get_client_list(void *pclient, unsigned char * pdata, UINT len){

	int nRet = 0;
	unsigned int resLen = 0;
	struct compute_node_info *pClients = NULL;
	unsigned int resNum = 0;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	//处理业务逻辑
	nRet = g_CrackBroker.GetClientList(&pClients,&resNum);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_DEBUG, "get_client_list: Broker ErrorCode %d\n",nRet);
	
	}else{
		CLog::Log(LOG_LEVEL_DEBUG, "get_client_list: Broker Get %d Clients OK\n",resNum);
		resLen = sizeof(struct compute_node_info) * resNum;
	}
	
	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_GET_CLIENT_LIST, nRet, pClients,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"get_client_list: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else{
		//CLog::Log(LOG_LEVEL_WARNING,"get_client_list: [%s:%d] Client Get Client Info OK\n",ip,port);
	}

	g_CrackBroker.Free(pClients);
	return nRet;
}

//download file res
// download file 
int cc_task_file_download(void *pclient, unsigned char * pdata,UINT len){

	CLog::Log(LOG_LEVEL_DEBUG, "file_download: this is file download start \n");
	int ret = 0;
	FILE *pfile = NULL;
	file_info fileinfo;
	download_file_req *preq = NULL;
	unsigned int filelen =  0;
	int resLen = 0;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	preq = (struct download_file_req *)pdata;
	char task_guid[40];
	if(g_CrackBroker.QueryTaskByWI(task_guid, (char *)preq->guid) != 0)
	{
		ret = -1;
		goto resp;
	}

	char filename[MAX_PATH];
	CCrackBroker::GetTaskFileByGuid(task_guid, filename, MAX_PATH);

	pfile = fopen(filename,"rb");
	if (!pfile){
		
		CLog::Log(LOG_LEVEL_WARNING,"file_download: fopen file %s error %d\n", filename, GetLastError());
		
		ret = -1;

	}else {
		fseek(pfile,0L,SEEK_END);
		filelen = ftell(pfile);  //获取文件长度
		fseek(pfile,0L,SEEK_SET);
				
		fileinfo.f = pfile;
		fileinfo.len = filelen;
		fileinfo.offset = 0;

		CLog::Log(LOG_LEVEL_NOMAL,"file_download: guid=%s\n", preq->guid);
		CLog::Log(LOG_LEVEL_NOMAL,"file_download: pfile=%p, len=%d\n",fileinfo.f,fileinfo.len);

		resLen = sizeof(struct file_info);
	}

resp:
	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_DOWNLOAD_FILE, ret, &fileinfo,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"file_download: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"file_download: [%s:%d] File Download #1 OK\n",ip,port);
	
	return ret;
}

//download file res tran
//download file start
int cc_task_file_download_start(void *pclient,unsigned char *pdata,UINT len){
	
	CLog::Log(LOG_LEVEL_DEBUG,"file_download_start: this is file download ...\n");
	int ret = 0;
	int rdlen = 0;
	int readLen = 0;
	FILE *pfile = NULL;
	BYTE resBuf[MAX_BUF_LEN];
	file_info *pFileInfo = NULL;

	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET sock = client->GetSocket();

	pFileInfo = (file_info *)pdata;
	pfile = (FILE *)pFileInfo->f;

	CLog::Log(LOG_LEVEL_NOMAL,"file_download_start: pfile=%p, len=%d\n",pFileInfo->f,pFileInfo->len);

	if (pFileInfo->len > 8196){		
		rdlen = 8196;
	}else{
		rdlen = pFileInfo->len;
	}

	while(!feof(pfile)){

		memset(resBuf,0,MAX_BUF_LEN);
		readLen  = fread(resBuf,1,rdlen,pfile);

		if (readLen < 0){

			CLog::Log(LOG_LEVEL_WARNING,"file_download_start: file read error %d.\n", GetLastError());
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

				CLog::Log(LOG_LEVEL_WARNING,"file_download_start: send data Error %d\n", ret);
				break;

			}else{
				CLog::Log(LOG_LEVEL_DEBUG,"file_download_start: sending data %d OK\n",readLen);
				readLen = 0;
			}
		}		
	}

	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"file_download_start: [%s:%d] Send Response Error %d\n",ip,port,ret);

	}else	
		CLog::Log(LOG_LEVEL_NOMAL,"file_download_start: [%s:%d] File Download #2 OK\n",ip,port);

	return ret;
}


//download file res end
//download file end
int cc_task_file_download_end(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_DEBUG,"file_download_end: this is file download end\n");
	int ret = 0;
	unsigned int resLen = 0;
	file_info *pFileInfo = NULL;
	file_info finfo;
	FILE *pfile = NULL;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();
	
	pFileInfo = (file_info *)pdata;
	pfile = (FILE *)pFileInfo->f;
	fclose(pfile);

	resLen = sizeof(struct file_info);
	
	memset(&finfo,0,sizeof(file_info));

	finfo.f = pfile;
	finfo.len = pFileInfo->len;
	finfo.offset = pFileInfo->offset;


	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_END_DOWNLOAD, 0, &finfo,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"file_download_end: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"file_download_end: [%s:%d] File Download #3 OK\n",ip,port);

	return ret;
}

//control client : upload start

//upload file req  
int cc_task_upload_file(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_DEBUG,"upload_file: this is file upload\n");
	int ret = 0;
	unsigned int resLen = 0;
	FILE *pfile = NULL;

	file_upload_res uploadres;
	file_upload_req *preq = NULL;
	char filename[MAX_PATH];
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	preq = (struct file_upload_req *)pdata;

	memset(&uploadres,0,sizeof(struct file_upload_res));
	memcpy(uploadres.guid,preq->guid,40);

	CCrackBroker::GetTaskFileByGuid((char *)uploadres.guid, filename, MAX_PATH);

	pfile = fopen((char *)filename,"wb");
	if (!pfile){
		CLog::Log(LOG_LEVEL_DEBUG,"upload_file: File %s open error\n",filename, GetLastError());	
		ret = -1;
		uploadres.f = NULL;

	}else{
		uploadres.f = pfile;
		open_files[pfile] = pfile;
		ret = 0;
	}

	uploadres.len = 0;
	uploadres.offset = 0;

	resLen = sizeof(struct file_upload_res);

	CLog::Log(LOG_LEVEL_NOMAL,"upload_file: guid=%s\n",uploadres.guid);
	CLog::Log(LOG_LEVEL_NOMAL,"upload_file: pfile=%p, len=%d\n",uploadres.f,uploadres.len);

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_UPLOAD_FILE, ret, &uploadres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"upload_file: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"upload_file: [%s:%d] File Upload #1 OK\n",ip,port);

	return ret;
}

//upload file start req  
int cc_task_upload_file_start(void *pclient,unsigned char *pdata,UINT len){
	
	CLog::Log(LOG_LEVEL_DEBUG,"upload_file_start: this is file upload .... \n");
	int ret = 0;
	FILE *pfile = NULL;

	file_upload_start_req *preq = NULL;
	unsigned int filelen =  0;
	unsigned int readLen = 0;
	unsigned int writeFileLen = 0;
	unsigned char cmd = 0;
	short status = 0;
	char filename[MAX_PATH];
	unsigned char resBuf[MAX_BUF_LEN];
	char tmpguid[40];
	
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET sock = client->GetSocket();
	
	if(len != sizeof(file_upload_start_req))
	{
		CLog::Log(LOG_LEVEL_DEBUG,"upload_file_start: invalid data\n");
		return -1;
	}

	preq = (file_upload_start_req *)pdata;
	filelen = preq->len;
	pfile = (FILE *)preq->f;
	
	if (!pfile || open_files.find(pfile) == open_files.end()){
		CLog::Log(LOG_LEVEL_WARNING,"upload_file_start: file is null\n");
		return -1;
	}

	memset(tmpguid,0,40);
	memcpy(tmpguid,preq->guid,40);

	CCrackBroker::GetTaskFileByGuid(tmpguid, filename, MAX_PATH);

	CLog::Log(LOG_LEVEL_NOMAL,"upload_file_start: filename=%s\n",filename);
	CLog::Log(LOG_LEVEL_NOMAL,"upload_file_start: pfile=%p, len=%d\n",preq->f,preq->len);

	unsigned int curlen = 0;
	
	while(curlen < filelen ){
		
		memset(resBuf,0,MAX_BUF_LEN);
		readLen = Read(sock,&cmd,&status,resBuf,MAX_BUF_LEN);
		CLog::Log(LOG_LEVEL_DEBUG, "upload_file_start: Recv cmd %d len %d\n", cmd, readLen);
		if(cmd != CMD_FILE_CONTENT)
		{
			CLog::Log(LOG_LEVEL_DEBUG, "upload_file_start: Recv unneed cmd %d\n", cmd);
			ret = ERR_INVALIDDATA;
			break;
		}

		if (readLen < 0 ){
			
			CLog::Log(LOG_LEVEL_WARNING,"upload_file_start: Recv client upload file %s %d\n",filename);
			ret = readLen;
			break;

		}else{

			curlen += readLen;
			//recv the file data 
			writeFileLen = fwrite(resBuf,1,readLen,pfile);
			if (writeFileLen < 1){
				CLog::Log(LOG_LEVEL_WARNING,"upload_file_start: [%s:%d] writefile Error %d\n",ip,port, GetLastError());
				ret = -3;
				break;

			}else{	
				//CLog::Log(LOG_LEVEL_DEBUG,"[%s:%d] Get File Upload %d: %d/%d OK\n",ip,port,writeFileLen, curlen, filelen);					
			}
		}
	}

	CLog::Log(LOG_LEVEL_NOMAL,"upload_file_start: File Upload #2 %s\n", (ret==0?"OK":"Error"));

	return 0;
}


//添加处理上传结束请求

int cc_task_upload_file_end(void *pclient,unsigned char *pdata,UINT len){

	CLog::Log(LOG_LEVEL_DEBUG,"upload_file_end: this is file upload End \n");
	int ret = 0;
	int nRet = 0;
	FILE *pfile = NULL;
	unsigned int resLen = 0;
	file_upload_end_req *preq = NULL;
	file_upload_end_res uploadres;
	unsigned int filelen = 0;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	char* owner = client->GetOwner();
	SOCKET cliSocket = client->GetSocket();
	
	preq = (struct file_upload_end_req *)pdata;
	memset(&uploadres,0,sizeof(struct file_upload_end_res));

	pfile = (FILE *)preq->f;

	std::map<void*, void*>::iterator it = open_files.find(pfile);
	if (!pfile || it == open_files.end()){	
		CLog::Log(LOG_LEVEL_WARNING,"upload_file_end: User %s request guid %s, file is NULL\n", owner, preq->guid);
		ret = -1;
		uploadres.f = NULL;
		resLen = 0;
	}else {
		open_files.erase(it);
		ret = 0;
		fclose(pfile);
		uploadres.f = pfile;
		pfile = NULL;
		resLen = sizeof(struct file_upload_end_res);

		CLog::Log(LOG_LEVEL_NOMAL,"upload_file_end: User %s request guid %s, Close File %p\n", owner, preq->guid,uploadres.f);
	}

	memcpy(uploadres.guid, preq->guid, 40);
	uploadres.len = preq->len;
	uploadres.offset = preq->offset;

	//修改流程，首先切分，如果出错，在返回切分错误原因
	if (ret == 0){	
		//通过任务guid 进行查找，并且分任务

		//处理业务逻辑
		//ret == -300 load file error 
		//ret == -301 split hash error
		ret = g_CrackBroker.SplitTask((char *)uploadres.guid);
		if (ret < 0){
			CLog::Log(LOG_LEVEL_WARNING, "upload_file_end: Failed to split task %s Error=%d\n", preq->guid, ret);
			
			//删除新创建任务
			nRet = g_CrackBroker.deleteTask((char *)uploadres.guid, pclient);
			if (nRet < 0 ){
				CLog::Log(LOG_LEVEL_WARNING,"upload_file_end: Delete Task %s Error %d\n",(char *)uploadres.guid,nRet);
				ret = nRet;  //删除任务错误
			}

		}else{
			CLog::Log(LOG_LEVEL_NOMAL, "upload_file_end: SplitTask %s OK\n", (char *)uploadres.guid, ret);
		}
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_END_UPLOAD, ret, &uploadres,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"upload_file_end: [%s, %s:%d] Send Response Error %d\n", owner, ip,port,m);
	}else
		CLog::Log(LOG_LEVEL_NOMAL,"upload_file_end: [%s, %s:%d] Upload Task #3 End OK\n", owner, ip,port);

	return ret;
}

//申请一个工作项
int comp_get_a_workitem(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_block *pcrackblock = NULL;
	unsigned int resLen = 0;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	char* owner = client->GetOwner();
	SOCKET cliSocket = client->GetSocket();

	//处理业务逻辑
	ret = g_CrackBroker.GetAWorkItem2(owner,&pcrackblock);
	if (ret < 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"get_workitem: [%s, %s:%d] Get error %d\n", owner, ip, port, ret);
		resLen = 0;	
	}else {
		CLog::Log(LOG_LEVEL_NOMAL, "get_workitem: [%s, %s:%d] Get OK [guid=%s, algo=%d, type=%d]\n", 
			owner, ip, port, pcrackblock->guid, pcrackblock->algo, pcrackblock->type);
		resLen = sizeof(struct crack_block);
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_GET_A_WORKITEM, ret, pcrackblock,resLen,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_WARNING,"get_workitem: [%s, %s:%d] Send Response Error %d\n",owner,ip,port,m);
		//发送失败,重置workitem
		if(resLen == sizeof(struct crack_block))
		{
			crack_result unlock = {0};
			unlock.status = WI_STATUS_UNLOCK;
			memcpy(unlock.guid, pcrackblock->guid, sizeof(unlock.guid));
			g_CrackBroker.GetWIResult(&unlock);
		}

	}else{
		CLog::Log(LOG_LEVEL_DEBUG,"get_workitem: [%s, %s:%d] Client Get A WorkItem OK\n",owner,ip,port);
	}
	g_CrackBroker.Free(pcrackblock);

	return ret;
}

//获得任务的状态
int comp_get_workitem_status(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_status *pstatus = NULL;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();
	pstatus = (struct crack_status *)pdata;

	ret = g_CrackBroker.GetWIStatus(pstatus);
	if (ret < 0 ){
		//CLog::Log(LOG_LEVEL_DEBUG,"get_item_status: Get Error %d\n", ret);
	}else {
		//CLog::Log(LOG_LEVEL_DEBUG,"get_item_status: [%s:%d] Get [guid=%s] OK\n", ip,port, pstatus->guid);
	}
	
	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_WORKITEM_STATUS, ret, NULL,0,true);
	if (m < 0){
		//CLog::Log(LOG_LEVEL_DEBUG,"get_item_status: [%s:%d] Send Response Error %d \n",ip,port,pstatus->guid,m);
	}else{
		//CLog::Log(LOG_LEVEL_DEBUG,"get_item_status: [%s:%d] item %s Progress OK\n",ip,port,pstatus->guid);
	}
	return ret;
}


//获得任务执行结果
int comp_get_workitem_res(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	struct crack_result *pres = NULL;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	pres = (struct crack_result *)pdata;
	ret = g_CrackBroker.GetWIResult(pres);
	if (ret < 0 ){
		CLog::Log(LOG_LEVEL_DEBUG,"get_item_res: [%s:%d] Get Result Error %d\n", ip,port, ret);
	}else {
		//CLog::Log(LOG_LEVEL_DEBUG,"Get A WorkItem Result OK\n");
	}

	//产生应答报文，并发送
	int m = Write(cliSocket, CMD_WORKITEM_RESULT, ret, NULL,0,true);
	if (m < 0){
		CLog::Log(LOG_LEVEL_DEBUG,"get_item_res: [%s:%d] Send Response Error %d \n",ip,port,m);
	}else{
		//CLog::Log(LOG_LEVEL_DEBUG,"get_item_res: [%s:%d] Client Item %s Result OK\n",ip,port,pres->guid);
	}
	return ret;
}

int client_quit(void *pclient,unsigned char *pdata,UINT len){

	int ret = 0;
	CClientInfo* client = (CClientInfo*)pclient;
	char* ip = client->GetIP();
	int port = client->GetPort();

	ret = g_CrackBroker.DoClientQuit(ip,port);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"client_quit: Can't find Client [%s:%d]\n",ip,port);

		ret = -1;
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"client_quit: [%s:%d] Quit OK\n",ip,port);
	}
	return ret;
}

//函数映射表出始化
static FUNC_MAP::value_type func_value_type[] ={

	FUNC_MAP::value_type(CMD_HEARTBEAT,client_keeplive),

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

	
	FUNC_MAP::value_type(CMD_DOWNLOAD_FILE,cc_task_file_download),  //下载命令处理
	FUNC_MAP::value_type(CMD_START_DOWNLOAD,cc_task_file_download_start),		 //开始下载
	FUNC_MAP::value_type(CMD_END_DOWNLOAD,cc_task_file_download_end),	 //下载结束 

	
	FUNC_MAP::value_type(CMD_UPLOAD_FILE,cc_task_upload_file),			//上传命令处理
	FUNC_MAP::value_type(CMD_START_UPLOAD,cc_task_upload_file_start),	//开始上传
	FUNC_MAP::value_type(CMD_END_UPLOAD,cc_task_upload_file_end),		//上传结束

	FUNC_MAP::value_type(CMD_CLIENT_QUIT,client_quit),		//处理客户端退出

};

static FUNC_MAP recv_data_map(func_value_type,func_value_type+sizeof(func_value_type)/sizeof(func_value_type[0]));


INT doRecvData(void* pclient, unsigned char* pdata, UINT len,BYTE cmdflag){
	if(recv_data_map.find(cmdflag) == recv_data_map.end())
		return -1;

	return (*recv_data_map[cmdflag])(pclient,pdata,len);
}