

#include "stdafx.h"
#include "PacketProcess.h"
#include "CLog.h"
#include "UnifBuffer.h"
#include "ClientLogin.h"
#include "Common.h"
#include "zlib.h"
#include "CmdBuf.h"
#include "ServerResp.h"
#include "guidgenerator.h"
#include "ClientManage.h"
#include "FileInfo.h"
#include "FileUploadManager.h"



CFileUploadManager g_FileUploadManager;

CClientManage g_ClientManage;

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

//超级控制节点登录处理
int doSuperLogin(void *pClient,unsigned char *pLogin,unsigned int len){
	
	
	INT nRet = 0;
	time_t tmptime;

	
	CLog::Log(LOG_LEVEL_WARNING,"Enter into Super login\n");
	
	SUPER_CTL_LOGIN supLogin;
	
	CopyMemory(&supLogin,pLogin,sizeof(SUPER_CTL_LOGIN));

	//客户端信息保存到客户端在线列表中
	 time(&tmptime);

	 ClientInfo *pClientInfo = new ClientInfo;
	
	 pClientInfo->m_LoginInfo.clientType = ROLE_SUPER_CONTROL;
	 pClientInfo->m_LoginInfo.cpuThreads = 0;
	 pClientInfo->m_LoginInfo.gpuThreads = 0;
	 CopyMemory(pClientInfo->m_LoginInfo.HostName,supLogin.HostName,50);
	 CopyMemory(&(pClientInfo->m_LoginInfo.guid),&(supLogin.guid),sizeof(GUID));
	 CopyMemory(&(pClientInfo->m_LoginInfo.OsVerInfoEx),&(supLogin.OsVerInfoEx),sizeof(OSVERSIONINFOEX));
	 CopyMemory(&(pClientInfo->m_LoginInfo.IPAddress),&(supLogin.IPAddress),sizeof(IN_ADDR));
	 pClientInfo->m_LoginInfo.socket = supLogin.socket;

	 pClientInfo->m_livetime = tmptime;
	 pClientInfo->m_ClientSock = supLogin.socket;
 
	// g_ClientList.push_back(pClientInfo);

	 g_ClientManage.InsertClient(pClientInfo);

	 ////////////////////////////////////
	//产生应答报文，并发送
	BYTE bToken = 0;
	bToken = CMD_LOGIN_OK;
	

	nRet = doSendData(pClient, &bToken, 1);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Create SUPER CONTRL Client Login Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Create SUPER CONTRL Client Login OK\n");
		nRet = 0;
	}

	return nRet;
}

//普通控制节点登录处理
int doControlLogin(void *pClient,unsigned char *pLogin,unsigned int len){

	
	INT nRet = 0;
	time_t tmptime;
	CLog::Log(LOG_LEVEL_WARNING,"Enter into Control login\n");

	NORMAL_CTL_LOGIN ctlLogin;
	

	CopyMemory(&ctlLogin,pLogin,sizeof(NORMAL_CTL_LOGIN));

	//客户端信息保存到在线列表中
	 time(&tmptime);

	 ClientInfo *pClientInfo = new ClientInfo;
	
	 pClientInfo->m_LoginInfo.clientType = ROLE_NORMAL_CONTROL;
	 pClientInfo->m_LoginInfo.cpuThreads = 0;
	 pClientInfo->m_LoginInfo.gpuThreads = 0;
	 CopyMemory(pClientInfo->m_LoginInfo.HostName,ctlLogin.HostName,50);
	 CopyMemory(&(pClientInfo->m_LoginInfo.guid),&(ctlLogin.guid),sizeof(GUID));
	 CopyMemory(&(pClientInfo->m_LoginInfo.OsVerInfoEx),&(ctlLogin.OsVerInfoEx),sizeof(OSVERSIONINFOEX));
	 CopyMemory(&(pClientInfo->m_LoginInfo.IPAddress),&(ctlLogin.IPAddress),sizeof(IN_ADDR));
	 pClientInfo->m_LoginInfo.socket = ctlLogin.socket;

	 pClientInfo->m_livetime = tmptime;
	 pClientInfo->m_ClientSock = ctlLogin.socket;
 
	 g_ClientManage.InsertClient(pClientInfo);
	// g_ClientList.push_back(pClientInfo);
	////////////////////////////////////

	//产生应答报文，并发送
	BYTE bToken = 0;
	bToken = CMD_LOGIN_OK;
	

	nRet = doSendData(pClient, &bToken, 1);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Create Normal CONTRL Client Login Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Create Normal CONTRL Client Login OK\n");
		nRet = 0;
	}

	return nRet;
}

//计算节点登录处理
int doCompLogin(void *pClient,unsigned char *pLogin,unsigned int len){


	INT nRet = 0;
	time_t tmptime;

	CLog::Log(LOG_LEVEL_WARNING,"Enter into computer login\n");
	
	COMPUTING_LOGIN compLogin;
	
	CopyMemory(&compLogin,pLogin,sizeof(COMPUTING_LOGIN));

	
	//客户端信息保存到在线列表中
	 time(&tmptime);

	 ClientInfo *pClientInfo = new ClientInfo;
	
	 pClientInfo->m_LoginInfo.clientType = ROLE_COMPUTE;
	 pClientInfo->m_LoginInfo.cpuThreads = compLogin.cpuThreads;
	 pClientInfo->m_LoginInfo.gpuThreads = compLogin.gpuThreads;
	 CopyMemory(pClientInfo->m_LoginInfo.HostName,compLogin.HostName,50);
	 CopyMemory(&(pClientInfo->m_LoginInfo.guid),&(compLogin.guid),sizeof(GUID));
	 CopyMemory(&(pClientInfo->m_LoginInfo.OsVerInfoEx),&(compLogin.OsVerInfoEx),sizeof(OSVERSIONINFOEX));
	 CopyMemory(&(pClientInfo->m_LoginInfo.IPAddress),&(compLogin.IPAddress),sizeof(IN_ADDR));
	 pClientInfo->m_LoginInfo.socket = compLogin.socket;

	 pClientInfo->m_livetime = tmptime;
	 pClientInfo->m_ClientSock = compLogin.socket;
 
	 g_ClientManage.InsertClient(pClientInfo);
	// g_ClientList.push_back(pClientInfo);
	////////////////////////////////////
	//产生应答报文，并发送
	BYTE bToken = 0;
	bToken = CMD_LOGIN_OK;
	

	nRet = doSendData(pClient, &bToken, 1);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Create Computing Client Login Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Create Computing Client Login OK\n");
		nRet = 0;
	}
	return nRet;
}

/*
 客户端登录请求结构
 BYTE bToken;   TOKEN_LOGIN
 BYTE bRoleFlag;	SUPERCONTROL,CONTROL,COMPUTER
 LOGIN_INFO  
*/
int client_login(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	CLog::Log(LOG_LEVEL_WARNING,"this is client login\n");
	BYTE bRoleFlag = pdata[1];
	switch(bRoleFlag){

	
		case ROLE_SUPER_CONTROL:

			nRet = doSuperLogin(pclient,&pdata[2],len-2);
			if (nRet != 0){
				
				CLog::Log(LOG_LEVEL_WARNING,"SuperControl Login Error\n");
				nRet = -1;

			}

			CLog::Log(LOG_LEVEL_WARNING,"SuperControl Login OK\n");
			break;
		case ROLE_NORMAL_CONTROL:


			nRet = doControlLogin(pclient,&pdata[2],len-2);
			if (nRet != 0){
				
				CLog::Log(LOG_LEVEL_WARNING,"Control Login Error\n");
				nRet = -2;

			}
			CLog::Log(LOG_LEVEL_WARNING,"Control Login OK\n");
			break;
		case ROLE_COMPUTE:

			nRet = doCompLogin(pclient,&pdata[2],len-2);
			if (nRet != 0){
				
				CLog::Log(LOG_LEVEL_WARNING,"Computer Login Error\n");
				nRet = -3;

			}
			CLog::Log(LOG_LEVEL_WARNING,"Computer Login OK\n");
			break;
		default :

			CLog::Log(LOG_LEVEL_WARNING,"Client Login Token Error\n");
			nRet = -4;
			break;

	}
	return nRet;
}





//同步心跳回应
int client_keeplive(void *pclient, unsigned char * pdata, UINT len){

	//send the result data
	INT nRet = 0;
	CLog::Log(LOG_LEVEL_WARNING,"This is a clieng keeplive\n");
	GUID guid;
	INT guidLen = 0;
	time_t tmpTime = 0;

	BYTE bToken = CMD_KEEPLIVE_OK;
	guidLen = sizeof(GUID);
	CopyMemory(&guid,&pdata[1],guidLen);
	CopyMemory(&tmpTime,&pdata[1+guidLen],sizeof(time_t));
	
	//查找GUID 的客户端节点 并 更新节点信息
	nRet = g_ClientManage.UpdateKeepliveInfo(guid,tmpTime);
	if (nRet != 0){

		std::string str ;
		guid_to_string(&guid,str);
		CLog::Log(LOG_LEVEL_WARNING,"UPdate the Keep Live time. client guid : %s\n",str);
		bToken = CMD_KEEPLIVE_ERR;
	}
	
	//生成应答并返回
	nRet = doSendData(pclient, &bToken, 1);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Client KeepLive Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Client KeepLive OK\n");
		nRet = 0;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//下边的为控制节点发送给服务器的请求
//任务upload 处理
int cc_task_upload(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task upload\n");
	int nRet = 0;
//	BYTE alg[ALG_TYPE_LEN];
//	BYTE hash[HASH_INFO_LEN];
//	BYTE ctlguid[GUID_LEN];
//	UINT algLen = hashLen = guidLen = 0;
	UINT sendLen = 0;
	char c_guid[48];
	LPBYTE pRecv = NULL;

	//读取upload 相关请求信息
	//pCmdAlg = pHashInfo = pGuid = NULL;

	CUnifBuffer unifBuf(&pdata[1],len-1);

	CCmdBuf ccmdalg(REQ_ALG_TYPE,unifBuf);
	CCmdBuf ccmdhash(REQ_HASH_INFO,unifBuf);
	CCmdBuf ccmdguid(REQ_CTL_GUID,unifBuf);
	
	//创建新任务，并将相关信息赋值给新任务
	memset(c_guid,0,48);
	gen_guid((unsigned char *)c_guid);
	 
	////////////////////////////////////

	//产生应答报文，并发送
	int guid_len = strlen(c_guid);
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_TASK_GUID,(BYTE *)c_guid,guid_len);
	
	sendLen = guid_len+9;
	pRecv = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pRecv){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Upload Send Buffer Error\n");
		return -1;
	}
	unifBuf.GetBuffer(&pRecv[1],sendLen-1);
	unifBuf.ResetBuffer();

	pRecv[0] = CMD_TASK_UPLOAD_OK;
	nRet = doSendData(pclient, pRecv, sendLen);
	if (nRet != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Upload Task Error\n");
		nRet = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Upload Task OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pRecv);
	return nRet;
}

//文件上传功能处理
int cc_task_file_upload(void *pclient, unsigned char * pdata,UINT len){

	int ret = 0;
	unsigned char bUpload;

	unsigned char *ptmpData = NULL;
	CLog::Log(LOG_LEVEL_WARNING,"this is task file upload\n");
	
	UINT sendLen = 0;
//	char c_guid[48];
	BYTE recvBuf[2];

	//读取upload 相关请求信息
	//pCmdAlg = pHashInfo = pGuid = NULL;
	bUpload = pdata[1];
	
	CUnifBuffer unifBuf(&pdata[2],len-2);
	CCmdBuf *pCmdFile = NULL;

	switch(bUpload){

		case REQ_TASK_UPLOAD_START:

			pCmdFile = new CCmdBuf(REQ_TASK_UPLOAD_START,unifBuf);

	//		ret = g_FileUploadManager.UploadStart((CltUploadFileStart *)pCmdFile->GetCmdData);

			delete pCmdFile;
			//g_FileUploadManager.SetFileStartInfo(
			break;
		case REQ_TASK_UPLOAD:

			
			pCmdFile = new CCmdBuf(REQ_TASK_UPLOAD,unifBuf);
		//	ret = g_FileUploadManager.UploadTransport((CltUploadFileBlockInfo *)pCmdFile->GetCmdData,pdata);

			delete pCmdFile;
			break;

		case REQ_TASK_UPLOAD_END:

			pCmdFile = new CCmdBuf(REQ_TASK_UPLOAD_END,unifBuf);
		//	ret = g_FileUploadManager.UploadEnd((CltUploadFileEnd*)pCmdFile->GetCmdData);

			delete pCmdFile;
			break;
		default:

			//类型不支持，直接返回
			return -1;

	}
	
	//产生应答报文，并发送
	memset(recvBuf,0,2);

	if (ret == 0){

		recvBuf[0] = CMD_TASK_FILE_UPLOAD_OK;
	}else{
		recvBuf[0] = CMD_TASK_FILE_UPLOAD_ERR;
	}
	recvBuf[1] = ret;
	
	sendLen = 2;
	ret = doSendData(pclient, recvBuf, 2);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Upload File Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Upload File OK\n");
		ret = 0;
	}

	return ret;

}


//任务开始指令处理
int cc_task_start(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task start\n");
	
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);
	CCmdBuf ccmdpri(REQ_TASK_PRIORITY,unifBuf);
	CCmdBuf ccmdstart(REQ_TASK_START,unifBuf);
	CCmdBuf ccmdend(REQ_TASK_END,unifBuf);
	CCmdBuf ccmdcharset(REQ_TASK_CHARSET,unifBuf);
	

	//根据任务guid 查找相关任务，并设置相关标记
	
	memset(cause,0,16);  //根据实际情况获取启动失败原因,模拟相关结果
	causeLen = 10;   
	 
	////////////////////////////////////

	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_TASK_START_CAUSE,cause,causeLen);
	

	sendLen = causeLen+9;

	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Start Send Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_TASK_START_ERR;

	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Start Task Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Start Task OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;

}




//任务停止指令处理
int cc_task_stop(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task stop\n");
	
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//根据任务guid 查找相关任务，并设置相关标记
	
	memset(cause,0,16);  //根据实际情况获取停止失败原因,模拟相关结果
	causeLen = 11;   
	 
	////////////////////////////////////

	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_TASK_STOP_CAUSE,cause,causeLen);
	

	sendLen = causeLen+9;

	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Stop Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_TASK_STOP_ERR;

	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;

}


//任务删除指令处理
int cc_task_delete(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task delete\n");

	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//根据任务guid 查找相关任务，并设置相关标记
	
	memset(cause,0,16);  //根据实际情况获取删除任务失败原因,模拟相关结果
	causeLen = 11;   
	 
	////////////////////////////////////

	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_TASK_DEL_CAUSE,cause,causeLen);
	

	sendLen = causeLen+9;

	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Delete Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_TASK_DEL_ERR;

	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Delete Task Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Delete Task OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;

}



//任务暂停指令处理
int cc_task_pause(void *pclient, unsigned char * pdata, UINT len){


	CLog::Log(LOG_LEVEL_WARNING,"this is task pause\n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//根据任务guid 查找相关任务，并设置相关标记
	
	memset(cause,0,16);  //根据实际情况获取暂停任务失败原因,模拟相关结果
	causeLen = 11;   
	 
	////////////////////////////////////

	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_TASK_PAUSE_CAUSE,cause,causeLen);
	

	sendLen = causeLen+9;

	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Task Pause Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_TASK_PAUSE_ERR;

	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;

}


//获取一个任务的执行结果
int cc_get_task_result(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is get a task result\n");
	
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	BYTE taskResult = 0;

	int pwdLen = 0;
	BYTE pwd[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//根据任务guid 查找相关任务，并设置相关标记
	
	//根据实际情况获取任务任务运行结果
	 
	taskResult = RES_TASK_FINISHED;
	memset(pwd,0,16);
	pwdLen = 12;

	////////////////////////////////////

	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	if (pwdLen == 0){
		
		//CMD_TASK_RESULT_OK + RES_TASK_RUNNING/FAILURE
		sendLen = pwdLen + 1 + 1;
	}else {
		
		//CMD_TASK_RESULT_OK + RES_TASK_FINISHED + RES_TASK_RESULT+ PasswordLen + Password
		sendLen = pwdLen + 1 + 1 + 8;

	}
 
	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Get Task Result Buffer Error\n");
		return -1;
	}

	//获得解密结果
	if (pwdLen > 0){

		unifBuf.GetBuffer(&pSend[2],sendLen-2);
		unifBuf.ResetBuffer();
	}

	pSend[0] = CMD_TASK_RESULT_OK;
	pSend[1] = taskResult;

	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;

}


//获取当前正在运行的任务的状态信息
int cc_refresh_status(void *pclient, unsigned char * pdata, UINT len){

	
	CLog::Log(LOG_LEVEL_WARNING,"this is refresh task status\n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pTaskStatus = NULL;
	UINT statusLen = 0;

	CUnifBuffer unifBuf;


	//获得当前任务列表，从中选择正在运行的任务
	//返回任务的运行状态相关信息
	
	statusLen = sizeof(TaskStatus) * 10;   //模拟10个正在计算的任务

	pTaskStatus = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,statusLen);
	if (!pTaskStatus){

		CLog::Log(LOG_LEVEL_WARNING,"Create Client Status Buffer Error\n");
		return -1;
	}
	
	ZeroMemory(pTaskStatus,statusLen);
		
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pTaskStatus);
	
	////////////////////////////////////

	//产生应答报文，并发送
	unifBuf.WriteItem(RES_TASK_STATUS,pTaskStatus,statusLen);
	
	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pTaskStatus);

	sendLen = statusLen + 1;

	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Get Tasks Status Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_REF_STATUS_OK;

	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;

}


//获取计算机节点信息
/*
 计算节点信息结构  ClientInfo


*/
int cc_get_client_list(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is get client list\n");

	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pClients = NULL;
	UINT nClientLen = 0;

	CUnifBuffer unifBuf;

	//获取当前在线计算节点
	pClients = (LPBYTE)g_ClientManage.GetCompNodes(&nClientLen);
	if (!pClients){

		CLog::Log(LOG_LEVEL_WARNING,"Get Computing Nodes NULL\n");
		nClientLen = 0;
		sendLen = 1;
	}else {

		CLog::Log(LOG_LEVEL_WARNING,"Get Computing Nodes %d\n",nClientLen);
		nClientLen = sizeof(ClientInfo)*nClientLen;
		unifBuf.WriteItem(RES_COMP_NODES_INFO,pClients,nClientLen);
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pClients);
		sendLen = nClientLen + 1;

	}

	////////////////////////////////////
	//产生应答报文，并发送
	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create Get Client List Buffer Error\n");
		return -1;
	}
	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_GET_CLTS_OK;
	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get Client List Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get Client List OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);

	return nRet;
}



//-----------------------------------------------------------------------------------
//下边为计算节点发送给服务器的请求
//返回workitem 运行结果
int comp_wi_recovered(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is get workitem recovered\n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pWIGuid = NULL;
	UINT nWiLen = 0;

		
	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdresult(REQ_DEC_RESULT_RECOVERED,unifBuf);
	CCmdBuf ccmdguid(REQ_COMP_NODE_GUID,unifBuf);

	//根据结果和计算节点guid获取相关的工作项guid
	nWiLen = sizeof(WorkItemGuid)* 5;
	pWIGuid = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nWiLen);
	if (!pWIGuid){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Guid Buffer Error\n");
		return -1;
	}
	
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIGuid);
	////////////////////////////////////
	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_WORKITEMS_GUID,pWIGuid,nWiLen);

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIGuid);

	sendLen = nWiLen + 1;
 
	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Guid Res Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_WI_RECOVERED_OK;
	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Guid Res Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Guid Res OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);
	return nRet;
}



//返回workitem 运行结果
int comp_wi_unrecovered(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is get workitem unrecovered\n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pWIGuid = NULL;
	UINT nWiLen = 0;

		
	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdresult(REQ_DEC_RESULT_UNRECOVERED,unifBuf);
	CCmdBuf ccmdguid(REQ_COMP_NODE_GUID,unifBuf);

	//根据结果和计算节点guid获取相关的工作项guid
	nWiLen = sizeof(WorkItemGuid)* 5;
	pWIGuid = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nWiLen);
	if (!pWIGuid){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Guid Unrecovered Buffer Error\n");
		return -1;
	}
	
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIGuid);
	////////////////////////////////////
	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_WORKITEMS_GUID,pWIGuid,nWiLen);

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIGuid);

	sendLen = nWiLen + 1;
 
	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Guid Unrecovered Res Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_WI_UNRECOVERED_OK;
	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Guid Unrecovered Res Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Guid Unrecovered Res OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);
	return nRet;

}


//获得指定计算节点上的workitem 信息
int comp_get_wi(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is Get a WorkItem \n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pWIInfo = NULL;
	UINT nWiInfoLen = 0;

		
	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_COMP_NODE_GUID,unifBuf);

	//根据结果和计算节点guid获取相关的工作项guid
	nWiInfoLen = sizeof(WorkItemInfo)* 5;
	pWIInfo = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nWiInfoLen);
	if (!pWIInfo){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Info Buffer Error\n");
		return -1;
	}
	
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIInfo);
	////////////////////////////////////
	//产生应答报文，并发送
	unifBuf.ResetBuffer();
	unifBuf.WriteItem(RES_WORKITEMS_INFO,pWIInfo,nWiInfoLen);

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIInfo);

	sendLen = nWiInfoLen + 1;
 
	pSend = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sendLen);
	if (!pSend){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Info Res Buffer Error\n");
		return -1;
	}

	unifBuf.GetBuffer(&pSend[1],sendLen-1);
	unifBuf.ResetBuffer();

	pSend[0] = CMD_WI_GET_OK;
	nRet = doSendData(pclient, pSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Info Res Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Info Res OK\n");
		nRet = 0;
	}

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSend);
	return nRet;
	

}

//返回workitem解密失败结果
int comp_wi_dec_failed(void *pclient, unsigned char * pdata, UINT len){


	CLog::Log(LOG_LEVEL_WARNING,"this is decrypt failed\n");
	int nRet = 0;
	UINT sendLen = 1;
	BYTE bSend = 0;
		
	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdresult(REQ_WORKITEM_GUID,unifBuf);
	CCmdBuf ccmdguid(REQ_COMP_NODE_GUID,unifBuf);

	//设置相关工作项和任务的状态
	

	////////////////////////////////////
	//产生应答报文，并发送
 	bSend = CMD_WI_FAILED_OK;
	nRet = doSendData(pclient, &bSend, sendLen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Failure Res Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Get WorkItem Failure Res OK\n");
		nRet = 0;
	}
	return nRet;

}




int comp_wi_download_file(void *pclient, unsigned char * pdata, UINT len){

	int ret = 0;
	unsigned char bDownload;
	UINT sendLen = 0;
	unsigned char recvBuf[2];
	unsigned char * pData = NULL;

	CltUploadFileStart *pUploadFile = NULL;
	

	CLog::Log(LOG_LEVEL_WARNING,"this is workitem file download\n");
	
	

	//读取upload 相关请求信息
	//pCmdAlg = pHashInfo = pGuid = NULL;
	bDownload = pdata[1];

	/*
	REQ_FILE_START,  //文件上传开始
	REQ_FILE_INFO,   //文件上传进行中
	REQ_FILE_END,	 //文件上传结束
*/
	
	CUnifBuffer unifBuf(&pdata[2],len-2);
	CCmdBuf *pCmdFile = NULL;

	switch(bDownload){

		case REQ_WORKITEM_DOWNLOAD_START:

			pCmdFile = new CCmdBuf(REQ_WORKITEM_DOWNLOAD_START,unifBuf);
			
		//	pUploadFile = g_FileUploadManager.DownLoadStart((CltDownloadFileStart *)pCmdFile->GetCmdData);

			delete pCmdFile;
			
			//g_FileUploadManager.SetFileStartInfo(
			break;
		case REQ_WORKITEM_DOWNLOAD:

			pCmdFile = new CCmdBuf(REQ_WORKITEM_DOWNLOAD,unifBuf);
		//	pData = g_FileUploadManager.DownLoadTransport((CltDownloadFileBlockInfo *)pCmdFile->GetCmdData);

			delete pCmdFile;
			break;

		case REQ_WORKITEM_DOWNLOAD_END:
			
			break;
		default:

			//类型不支持，直接返回
			return -1;

	}
	
	//产生应答报文，并发送
	

	memset(recvBuf,0,2);
	if (ret == 0){

		recvBuf[0] = CMD_WI_FILE_DOWNLAOD_OK;
	}else{
		recvBuf[0] = CMD_WI_FILE_DOWNLOAD_ERR;
	}
	recvBuf[1] = ret;
	
	sendLen = 2;
	ret = doSendData(pclient, recvBuf, 2);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"Downlaod File Error\n");
		ret = -2;
	}else{
		CLog::Log(LOG_LEVEL_WARNING,"Download File OK\n");
		ret = 0;
	}

	return ret;

}


static int (*recv_data_done[])(void *pclient, unsigned char * pdata, unsigned int len) = {

	//control and computer all send request
	client_login,
	client_keeplive,

	//control send command
	cc_task_upload,
	cc_task_file_upload,  //任务文件上传

	cc_task_start,
	cc_task_stop,
	cc_task_delete,
	cc_task_pause,
	cc_get_task_result,
	cc_refresh_status,
	cc_get_client_list,

	//computer send request
	comp_wi_recovered,
	comp_wi_unrecovered,
	comp_get_wi,
	comp_wi_dec_failed,

	comp_wi_download_file,

};





INT doRecvData(LPVOID pclient, LPBYTE pdata, UINT len,BYTE cmdflag){


	return (*recv_data_done[cmdflag])(pclient,pdata,len);
}


