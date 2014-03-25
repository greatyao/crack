

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

//������ݵ���װ
int doSendData(void *pClient,unsigned char *pdata,unsigned int len){

	int nRet = 0;
	LPBYTE pSendBuf = NULL;
	UINT nTotal = 0;
	UINT nCompressLen = 0;
	unsigned long lcomlen = 0;

	//�����÷������ݳ���
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

	
	//���÷��ͽӿڣ����͸��Զ�
	nRet = SendDataToPeer(pClient, pSendBuf, nTotal);
	if (nRet != 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Send Data Error\n");
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pSendBuf);
		return -3;

	}


	CLog::Log(LOG_LEVEL_WARNING,"Send Data OK\n");
	return 0;


}

//�������ƽڵ��¼����
int doSuperLogin(void *pClient,unsigned char *pLogin,unsigned int len){
	
	
	INT nRet = 0;
	time_t tmptime;

	
	CLog::Log(LOG_LEVEL_WARNING,"Enter into Super login\n");
	
	SUPER_CTL_LOGIN supLogin;
	
	CopyMemory(&supLogin,pLogin,sizeof(SUPER_CTL_LOGIN));

	//�ͻ�����Ϣ���浽�ͻ��������б���
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
	//����Ӧ���ģ�������
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

//��ͨ���ƽڵ��¼����
int doControlLogin(void *pClient,unsigned char *pLogin,unsigned int len){

	
	INT nRet = 0;
	time_t tmptime;
	CLog::Log(LOG_LEVEL_WARNING,"Enter into Control login\n");

	NORMAL_CTL_LOGIN ctlLogin;
	

	CopyMemory(&ctlLogin,pLogin,sizeof(NORMAL_CTL_LOGIN));

	//�ͻ�����Ϣ���浽�����б���
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

	//����Ӧ���ģ�������
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

//����ڵ��¼����
int doCompLogin(void *pClient,unsigned char *pLogin,unsigned int len){


	INT nRet = 0;
	time_t tmptime;

	CLog::Log(LOG_LEVEL_WARNING,"Enter into computer login\n");
	
	COMPUTING_LOGIN compLogin;
	
	CopyMemory(&compLogin,pLogin,sizeof(COMPUTING_LOGIN));

	
	//�ͻ�����Ϣ���浽�����б���
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
	//����Ӧ���ģ�������
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
 �ͻ��˵�¼����ṹ
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





//ͬ��������Ӧ
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
	
	//����GUID �Ŀͻ��˽ڵ� �� ���½ڵ���Ϣ
	nRet = g_ClientManage.UpdateKeepliveInfo(guid,tmpTime);
	if (nRet != 0){

		std::string str ;
		guid_to_string(&guid,str);
		CLog::Log(LOG_LEVEL_WARNING,"UPdate the Keep Live time. client guid : %s\n",str);
		bToken = CMD_KEEPLIVE_ERR;
	}
	
	//����Ӧ�𲢷���
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
//�±ߵ�Ϊ���ƽڵ㷢�͸�������������
//����upload ����
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

	//��ȡupload ���������Ϣ
	//pCmdAlg = pHashInfo = pGuid = NULL;

	CUnifBuffer unifBuf(&pdata[1],len-1);

	CCmdBuf ccmdalg(REQ_ALG_TYPE,unifBuf);
	CCmdBuf ccmdhash(REQ_HASH_INFO,unifBuf);
	CCmdBuf ccmdguid(REQ_CTL_GUID,unifBuf);
	
	//���������񣬲��������Ϣ��ֵ��������
	memset(c_guid,0,48);
	gen_guid((unsigned char *)c_guid);
	 
	////////////////////////////////////

	//����Ӧ���ģ�������
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

//�ļ��ϴ����ܴ���
int cc_task_file_upload(void *pclient, unsigned char * pdata,UINT len){

	int ret = 0;
	unsigned char bUpload;

	unsigned char *ptmpData = NULL;
	CLog::Log(LOG_LEVEL_WARNING,"this is task file upload\n");
	
	UINT sendLen = 0;
//	char c_guid[48];
	BYTE recvBuf[2];

	//��ȡupload ���������Ϣ
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

			//���Ͳ�֧�֣�ֱ�ӷ���
			return -1;

	}
	
	//����Ӧ���ģ�������
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


//����ʼָ���
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
	

	//��������guid ����������񣬲�������ر��
	
	memset(cause,0,16);  //����ʵ�������ȡ����ʧ��ԭ��,ģ����ؽ��
	causeLen = 10;   
	 
	////////////////////////////////////

	//����Ӧ���ģ�������
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




//����ָֹͣ���
int cc_task_stop(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task stop\n");
	
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//��������guid ����������񣬲�������ر��
	
	memset(cause,0,16);  //����ʵ�������ȡֹͣʧ��ԭ��,ģ����ؽ��
	causeLen = 11;   
	 
	////////////////////////////////////

	//����Ӧ���ģ�������
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


//����ɾ��ָ���
int cc_task_delete(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is task delete\n");

	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//��������guid ����������񣬲�������ر��
	
	memset(cause,0,16);  //����ʵ�������ȡɾ������ʧ��ԭ��,ģ����ؽ��
	causeLen = 11;   
	 
	////////////////////////////////////

	//����Ӧ���ģ�������
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



//������ָͣ���
int cc_task_pause(void *pclient, unsigned char * pdata, UINT len){


	CLog::Log(LOG_LEVEL_WARNING,"this is task pause\n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	int causeLen = 0;
	BYTE cause[16];

	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_TASK_GUID,unifBuf);


	//��������guid ����������񣬲�������ر��
	
	memset(cause,0,16);  //����ʵ�������ȡ��ͣ����ʧ��ԭ��,ģ����ؽ��
	causeLen = 11;   
	 
	////////////////////////////////////

	//����Ӧ���ģ�������
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


//��ȡһ�������ִ�н��
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


	//��������guid ����������񣬲�������ر��
	
	//����ʵ�������ȡ�����������н��
	 
	taskResult = RES_TASK_FINISHED;
	memset(pwd,0,16);
	pwdLen = 12;

	////////////////////////////////////

	//����Ӧ���ģ�������
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

	//��ý��ܽ��
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


//��ȡ��ǰ�������е������״̬��Ϣ
int cc_refresh_status(void *pclient, unsigned char * pdata, UINT len){

	
	CLog::Log(LOG_LEVEL_WARNING,"this is refresh task status\n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pTaskStatus = NULL;
	UINT statusLen = 0;

	CUnifBuffer unifBuf;


	//��õ�ǰ�����б�����ѡ���������е�����
	//�������������״̬�����Ϣ
	
	statusLen = sizeof(TaskStatus) * 10;   //ģ��10�����ڼ��������

	pTaskStatus = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,statusLen);
	if (!pTaskStatus){

		CLog::Log(LOG_LEVEL_WARNING,"Create Client Status Buffer Error\n");
		return -1;
	}
	
	ZeroMemory(pTaskStatus,statusLen);
		
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pTaskStatus);
	
	////////////////////////////////////

	//����Ӧ���ģ�������
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


//��ȡ������ڵ���Ϣ
/*
 ����ڵ���Ϣ�ṹ  ClientInfo


*/
int cc_get_client_list(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is get client list\n");

	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pClients = NULL;
	UINT nClientLen = 0;

	CUnifBuffer unifBuf;

	//��ȡ��ǰ���߼���ڵ�
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
	//����Ӧ���ģ�������
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
//�±�Ϊ����ڵ㷢�͸�������������
//����workitem ���н��
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

	//���ݽ���ͼ���ڵ�guid��ȡ��صĹ�����guid
	nWiLen = sizeof(WorkItemGuid)* 5;
	pWIGuid = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nWiLen);
	if (!pWIGuid){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Guid Buffer Error\n");
		return -1;
	}
	
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIGuid);
	////////////////////////////////////
	//����Ӧ���ģ�������
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



//����workitem ���н��
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

	//���ݽ���ͼ���ڵ�guid��ȡ��صĹ�����guid
	nWiLen = sizeof(WorkItemGuid)* 5;
	pWIGuid = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nWiLen);
	if (!pWIGuid){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Guid Unrecovered Buffer Error\n");
		return -1;
	}
	
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIGuid);
	////////////////////////////////////
	//����Ӧ���ģ�������
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


//���ָ������ڵ��ϵ�workitem ��Ϣ
int comp_get_wi(void *pclient, unsigned char * pdata, UINT len){

	CLog::Log(LOG_LEVEL_WARNING,"this is Get a WorkItem \n");
	int nRet = 0;
	UINT sendLen = 0;
	LPBYTE pSend = NULL;
	
	LPBYTE pWIInfo = NULL;
	UINT nWiInfoLen = 0;

		
	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdguid(REQ_COMP_NODE_GUID,unifBuf);

	//���ݽ���ͼ���ڵ�guid��ȡ��صĹ�����guid
	nWiInfoLen = sizeof(WorkItemInfo)* 5;
	pWIInfo = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nWiInfoLen);
	if (!pWIInfo){

		CLog::Log(LOG_LEVEL_WARNING,"Create WorkItem Info Buffer Error\n");
		return -1;
	}
	
	//HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pWIInfo);
	////////////////////////////////////
	//����Ӧ���ģ�������
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

//����workitem����ʧ�ܽ��
int comp_wi_dec_failed(void *pclient, unsigned char * pdata, UINT len){


	CLog::Log(LOG_LEVEL_WARNING,"this is decrypt failed\n");
	int nRet = 0;
	UINT sendLen = 1;
	BYTE bSend = 0;
		
	CUnifBuffer unifBuf(&pdata[1],len-1);
	CCmdBuf ccmdresult(REQ_WORKITEM_GUID,unifBuf);
	CCmdBuf ccmdguid(REQ_COMP_NODE_GUID,unifBuf);

	//������ع�����������״̬
	

	////////////////////////////////////
	//����Ӧ���ģ�������
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
	
	

	//��ȡupload ���������Ϣ
	//pCmdAlg = pHashInfo = pGuid = NULL;
	bDownload = pdata[1];

	/*
	REQ_FILE_START,  //�ļ��ϴ���ʼ
	REQ_FILE_INFO,   //�ļ��ϴ�������
	REQ_FILE_END,	 //�ļ��ϴ�����
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

			//���Ͳ�֧�֣�ֱ�ӷ���
			return -1;

	}
	
	//����Ӧ���ģ�������
	

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
	cc_task_file_upload,  //�����ļ��ϴ�

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


