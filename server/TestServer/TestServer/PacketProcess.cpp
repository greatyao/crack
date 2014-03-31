

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
#include "macros.h"

#include "algorithm_types.h"

#include "ReqPacket.h"
#include "ResPacket.h"
#include "CrackBroker.h"
#include <stdio.h>


#define FILE_DIR ".\\tempdir\\"

//CFileUploadManager g_FileUploadManager;

CClientManage g_ClientManage;


static CCrackBroker g_CrackBroker;

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

//超级控制节点登录处理
int doSuperLogin(void *pClient,unsigned char *pLogin,unsigned int len){
	
	
	INT nRet = 0;
	control_header cltHeader = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
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


	 /*
	BYTE bToken = 0;
	bToken = CMD_LOGIN_OK;
	


	nRet = doSendData(pClient, &bToken, 1);
	*/

	nRet = doSendDataNew(pClient,(unsigned char *)&cltHeader,sizeof(control_header));
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
	control_header cltHeader = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
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
	/*
	BYTE bToken = 0;
	bToken = CMD_LOGIN_OK;
	

	nRet = doSendData(pClient, &bToken, 1);
	*/
	 
    nRet = doSendDataNew(pClient,(unsigned char *)&cltHeader,sizeof(control_header));
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
	control_header cltHeader = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
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
/*	BYTE bToken = 0;
	bToken = CMD_LOGIN_OK;
	

	nRet = doSendData(pClient, &bToken, 1);
*/
	nRet = doSendDataNew(pClient,(unsigned char *)&cltHeader,sizeof(control_header)); 
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
	BYTE bRoleFlag = pdata[1];  //类型标识
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
	control_header replyHdr = INITIALIZE_EMPTY_HEADER(TOKEN_HEARTBEAT);
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
	//	guid_to_string(&guid,str);
		CLog::Log(LOG_LEVEL_WARNING,"UPdate the Keep Live time. client guid : %s\n",str);

		replyHdr.response = CMD_KEEPLIVE_ERR;
	//	bToken = CMD_KEEPLIVE_ERR;
	}
	
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

//--------------------------------------------------------------------------
//下边的为控制节点发送给服务器的请求

//new upload for test
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
	//gen_guid((unsigned char *)c_guid);
	 
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
	control_header cltHeader = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);
	CLog::Log(LOG_LEVEL_WARNING,"Enter into Login\n");

	client_login_req *pC = (struct client_login_req *)pdata;
	client_login_req myclient;
	
	if (pclient == NULL){
		
		memset(&myclient,0,sizeof(struct client_login_req));
		memset(buf,0,40);
		sprintf(buf,"%u",*(SOCKET *)pclient);
		myclient.m_clientsock = *(SOCKET *)pclient;
		myclient.m_cputhreads = myclient.m_gputhreads = 0;
		myclient.m_type = 0;
		memcpy(myclient.m_guid,buf,strlen(buf));
		pC = &myclient;
		
	}

	//处理业务逻辑

	nRet = g_CrackBroker.ClientLogin(pC);
	if (nRet < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Client Login --Error\n");
	

	}else {
		
		CLog::Log(LOG_LEVEL_WARNING,"Client Login ---OK\n");

	}


	////////////////////////////////////

	//产生应答报文，并发送
	cltHeader.response = nRet;
		 
    nRet = doSendDataNew(pclient,(unsigned char *)&cltHeader,sizeof(control_header));
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Client Login Error\n");
		nRet = -2;

	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Client Login OK\n");
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
	//nRet = g_CrackBroker.ClientKeepLive(pKeeplive);



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


	pfile = fopen((char *)puploadstartreq->guid,"ab");
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

	//computing send request
	comp_wi_status,
	comp_wi_result,

	comp_wi_download_file,

};

INT doRecvData(LPVOID pclient, LPBYTE pdata, UINT len,BYTE cmdflag){


	return (*recv_data_map[cmdflag])(pclient,pdata,len);
	//return (*recv_data_done[cmdflag])(pclient,pdata,len);
}


