

#include "stdafx.h"

#include "macros.h"
#include "algorithm_types.h"
#include "err.h"
#include "Common.h"
#include "server_function.h"
#include "CLog.h"
#include "PacketProcess.h"
#include "zlib.h"
#include "ServerResp.h"
#include "err.h"
#include "ServerResp.h"

#pragma comment(lib,"zlib.lib")


VOID ProcessClientData1(LPVOID lpParameter){

	SOCKET cliSocket = *(SOCKET *)lpParameter;
	
	INT nRet = 0;
	UINT len = 0;

	BYTE recvBuf[MAX_BUF_LEN*4];
	INT cmdheader = sizeof(control_header);
	unsigned char cmd;
	short status;

	struct sockaddr_in addr;
	int len2;
	getpeername(cliSocket, (sockaddr *)&addr, &len2);
	char ip[16];
	strcpy(ip, inet_ntoa(addr.sin_addr));

	while(1)
	{
		int m = Read(cliSocket, &cmd, &status, recvBuf, sizeof(recvBuf));
		CLog::Log(LOG_LEVEL_WARNING, "%s recv cmd %d status %d body %d\n",ip, cmd, status, m);

		if(m == ERR_CONNECTIONLOST) break;//推出了

		doRecvData(lpParameter, recvBuf, m, cmd);
	}

	CLog::Log(LOG_LEVEL_WARNING, "Quit %s %d\n", __FUNCTION__, cliSocket);

}



VOID ProcessClientData(LPVOID lpParameter){

	SOCKET cliSocket = *(SOCKET *)lpParameter;
	
	INT nRet = 0;
	UINT len = 0;
	UINT hdrLen = 0;

	BYTE recvBuf[MAX_BUF_LEN];
	hdrLen = sizeof(control_header);

	
	while(true){

		memset(recvBuf,0,MAX_BUF_LEN);

/*		nRet = recv(cliSocket,(char *)recvBuf,hdrLen,0);
		//nRet = recv(cliSocket,(char *)recvBuf,MAX_BUF_LEN,0);
		if(nRet == 0 || nRet == SOCKET_ERROR){

			CLog::Log(LOG_LEVEL_WARNING,"Client %d Quit.\n",cliSocket);
//			printf("Client Quit!\n");
	
	//		g_Server->m_nCurrentThread -=1;
	//		连接数减少
			
			break;
		}else{
			
			len = nRet;
			CLog::Log(LOG_LEVEL_WARNING,"Recv Data Len :%d.\n",len);
		//	printf("Recv Len : %d \n",nRet);


		}
*/	
		

		nRet = RecvDataFromPeer(lpParameter,recvBuf,hdrLen);
		if (nRet < 0 ){

			CLog::Log(LOG_LEVEL_WARNING,"Client %d Quit.\n",cliSocket);
		//	closesocket(cliSocket);
			continue;
			//return ;

		}else{

			
			len = hdrLen;
			CLog::Log(LOG_LEVEL_WARNING,"Recv Data Len :%d.\n",len);

		}

		
		unsigned int dataLen = 

		//gen resposne data, then send to client
		//Old the Recv and send 
		nRet = SendServerData((LPVOID)&cliSocket,recvBuf,len);
		if (nRet != 0){

			CLog::Log(LOG_LEVEL_WARNING,"Server Process Data Error\n");
		//	closesocket(cliSocket);
		//	break;
		}

	}
	return ;
}

/*
unsigned char magic[5];			//G&CPU
	unsigned char cmd;				//命令字
	short response;					//回应状态
	unsigned int dataLen;			//原始数据长度
	unsigned int compressLen;		//压缩数据后长度
*/

INT SendServerData2(LPVOID pClient,LPBYTE pData,UINT len){

	
	SOCKET cliSocket = *(SOCKET *)pClient;
	control_header controlHeader;
	control_header resheader;
	unsigned char recvBuf[1024];
	unsigned char uncompbuf[1024];
	unsigned long uncomplen = 0;
	unsigned long complen = 0;

	int ret = 0;

	unsigned char cmd = 0;

	memcpy(&controlHeader,pData,sizeof(control_header));

	cmd = controlHeader.cmd;

	CLog::Log(LOG_LEVEL_WARNING,"Recv CMD %d\n",cmd);


	switch(controlHeader.cmd){

			
		case TOKEN_HEARTBEAT:
			
			CLog::Log(LOG_LEVEL_WARNING,"Token HeatBeat Recv\n");

			resheader.cmd = COMMAND_REPLAY_HEARTBEAT;
			memcpy(resheader.magic,BUFF_TAG,5);
			
			resheader.compressLen = resheader.dataLen=resheader.response = 0;


			SimpleSendData(pClient,(LPBYTE)&resheader,sizeof(control_header),0);



			break;
		case TOKEN_LOGIN:

			CLog::Log(LOG_LEVEL_WARNING,"Token Login Recv\n");

			SimpleSendData(pClient,(LPBYTE)&controlHeader,sizeof(control_header),1);
			break;

		case CMD_GET_A_WORKITEM:
			
			{
				printf("获取计算任务\n");

			//	resheader =INITIALIZE_EMPTY_HEADER(104);

				

				static crack_block item1 = {algo_sha1,		charset_num,	bruteforce,  0, "0x0001", "8cb2237d0679ca88db6464eac60da96345513964", 1, 7, 0, 0},
				item2 = {algo_lm,		charset_num,	bruteforce,  0, "0x0002", "AEBD4DE384C7EC43AAD3B435B51404EE", 1, 6, 0, 0},
				item3 = {algo_oscommerce,charset_num,	bruteforce,  0, "0x0004", "d6b0ab7f1c8ab8f514db9a6d85de160a:abc", 1, 6, 0, 0},
				item4 = {algo_md5,		charset_num,	bruteforce,  0, "0x0008", "827ccb0eea8a706c4c34a16891f84e7b", 1, 6, 0, 0},
				item5 = {algo_md5,		charset_lalphanum,	bruteforce,  0, "0x0018", "014735b4462b68f44dbf8f1525d39b2b", 1, 6, 0, 0},
				item6 = {algo_desunix,	charset_num,	bruteforce,  0,	"0x0010", "27EP4PuToKUSI", 1, 6, 0, 0},
				item7 = {algo_desunix,	charset_num,	bruteforce,  0,	"0x0012", "27EP4PuToKUSi", 1, 6, 0, 0};
				
				const int NN = 7;
				static crack_block* all_items = (crack_block*)malloc(sizeof(crack_block)*NN);
				all_items[0] = item1;
				all_items[1] = item2;
				all_items[2] = item3;
				all_items[3] = item4;
				all_items[4] = item5;
				all_items[5] = item6;
				all_items[6] = item7;

				
				static int mm = 0;
				BYTE	bToken = CMD_GET_A_WORKITEM;
			/*	char buffer[sizeof(crack_block)+1];
				memcpy(buffer, &bToken, sizeof(bToken));
				memcpy(buffer+1, &all_items[mm], sizeof(crack_block));
				*/

		
			//	SimpleSendData(pClient,(LPBYTE)&,sizeof(control_header),1);

				SimpleSendData(pClient,(LPBYTE)&all_items[(mm++)%7],sizeof(crack_block),104);
			
				break;
			}
		case CMD_WORKITEM_STATUS:
			{

				memset(recvBuf,0,1024);

				memset(uncompbuf,0,1024);


				ret = recv(cliSocket,(char *)recvBuf,1024,0);
				if (ret < 0 ){

					CLog::Log(LOG_LEVEL_WARNING,"Recv Get A WorkItem Error\n");
					return -1;

				}


				uncomplen = controlHeader.dataLen;
				complen = controlHeader.compressLen;
				//uncompress
				ret = uncompress(uncompbuf,&uncomplen,recvBuf,complen);
				if (ret != 0 ){
					
					CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
					return -3;

				}

				//end of uncompress


				crack_status* status = (crack_status*)(uncompbuf);
				CLog::Log(LOG_LEVEL_WARNING,"%s: 进度%d 速度%f 时间%u\n", status->guid, status->progress, status->speed, status->remainTime);
			}
			break;

		case CMD_WORKITEM_RESULT:
			{
				memset(recvBuf,0,1024);
				memset(uncompbuf,0,1024);
				ret = recv(cliSocket,(char *)recvBuf,1024,0);
				if (ret < 0 ){

					CLog::Log(LOG_LEVEL_WARNING,"Recv Get A WorkItem Error\n");
					return -1;

				}

				//uncompress data


				uncomplen = controlHeader.dataLen;
				complen = controlHeader.compressLen;
				//uncompress
				ret = uncompress(uncompbuf,&uncomplen,recvBuf,complen);
				if (ret != 0 ){
					
					CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
					return -3;

				}

				
				//end of uncompress

				crack_result* result = (crack_result*)(uncompbuf);
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
				
			}

			break;

		default :
			return -1;
	}

	return 0;
}




INT SimpleSendData(LPVOID pClient,LPBYTE pdata, UINT len,int tag){


	SOCKET cliSocket = *(SOCKET *)pClient;

	control_header ctlheader;
	control_header *pcontrolheader = NULL;
	int ret = 0;
	
	/*BYTE sendBuf[MAX_BUF_LEN];

	memset(sendBuf,0,MAX_BUF_LEN);

	memcpy(sendBuf,pData,len);
	
	*/


	//data compress 


	int nRet = 0;
	LPBYTE pSendBuf = NULL;
	unsigned char tmpdata[5];
	unsigned char compdata[100];
	unsigned char tmpcomp[1024];

	UINT nTotal = 0;
	unsigned long nCompressLen = 0;
	unsigned long lcomlen = 0;
	unsigned long uncomplen = 5;

	if (tag == 1){

		ret = send(cliSocket,(char *)pdata,len,0);
		if (ret != len){
				
			CLog::Log(LOG_LEVEL_WARNING,"Send reply Error\n");
			return -1;

		}

		return 0;

	}

	if (tag == 104){



		//memset(tmpdata,0,5);
		memset(tmpcomp,0,1024);
		nCompressLen = 1024;
	
		
		//计算获得发送数据长度
		//nCompressLen = (float)uncomplen * 1.001+12;
	
		nRet = compress(tmpcomp,&nCompressLen,pdata,len);
		if (nRet != 0){

			CLog::Log(LOG_LEVEL_WARNING,"Compress 111 Send Buff Error\n");
			return -2;

		}

		CLog::Log(LOG_LEVEL_WARNING,"Compress 104 Data complen %d Error\n",nCompressLen);
	//	nCompressLen = lcomlen;

		
		//end of compress

		ctlheader.cmd = 104;
			memcpy(ctlheader.magic,BUFF_TAG,5);
			
			ctlheader.compressLen = ctlheader.dataLen=ctlheader.response = 0;
			ctlheader.compressLen = nCompressLen;
		ctlheader.dataLen = len;

	/*	ctlheader = INITIALIZE_EMPTY_HEADER(104);

		ctlheader.compressLen = nCompressLen;
		ctlheader.dataLen = len;

		ctlheader.response = 0;
		*/

		ret = send(cliSocket,(char *)&ctlheader,sizeof(control_header),0);
		if (ret != sizeof(control_header)){
				
			CLog::Log(LOG_LEVEL_WARNING,"Send 104 reply Error\n");
			return -1;

		}


		ret = send(cliSocket,(char *)tmpcomp,nCompressLen,0);
		if (ret != nCompressLen){
				
			CLog::Log(LOG_LEVEL_WARNING,"Send 104 Data Error\n");
			return -1;

		}

		CLog::Log(LOG_LEVEL_WARNING,"Send  104 Data len %d Error\n",nCompressLen);


		CLog::Log(LOG_LEVEL_WARNING,"Send 104 Data OK\n");





		return 0;
	}

	memset(tmpdata,0,5);
	memset(compdata,0,100);

	memcpy(tmpdata,"hello",5);
	uncomplen = 5;
	
	//计算获得发送数据长度
	//nCompressLen = (float)uncomplen * 1.001+12;
	nCompressLen = 100;

	nRet = compress(compdata,&nCompressLen,tmpdata,uncomplen);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -2;

	}

	CLog::Log(LOG_LEVEL_WARNING,"Compress Data complen %d \n",nCompressLen);
//	nCompressLen = lcomlen;

	
	//end of compress
	pcontrolheader = (control_header *)pdata;

	pcontrolheader->compressLen = nCompressLen;
	pcontrolheader->dataLen = uncomplen;

	pcontrolheader->response = 0;
	

	ret = send(cliSocket,(char *)pdata,len,0);
	if (ret != len){
			
		CLog::Log(LOG_LEVEL_WARNING,"Send reply Error\n");
		return -1;

	}


	ret = send(cliSocket,(char *)compdata,nCompressLen,0);
	if (ret != len){
			
		CLog::Log(LOG_LEVEL_WARNING,"Send Data Error\n");
		return -1;

	}

	CLog::Log(LOG_LEVEL_WARNING,"Send  Data len %d Error\n",nCompressLen);

	CLog::Log(LOG_LEVEL_WARNING,"Send Data OK\n");
	return 0;
}


INT SendServerData(LPVOID pClient, LPBYTE pData, UINT len)
{

	UINT nRet = 0;
	control_header *pCtlHeader = NULL;
	BYTE recvBuf[MAX_BUF_LEN];
	BYTE unCompressBuf[MAX_BUF_LEN];
	int ctlHdrLen = 0;
	BYTE bCmd = 0 ;
	unsigned long uOrgLen = 0;
	unsigned long uCompLen = 0;
	
	//use new header
	SOCKET cliSocket = *(SOCKET *)pClient;
	pCtlHeader = (control_header*)pData;
	/*
	unsigned char magic[5];			//G&CPU
	unsigned char cmd;				//命令字
	short response;					//回应状态
	unsigned int dataLen;			//原始数据长度
	unsigned int compressLen;		//压缩数据后长度
	*/

	memset(recvBuf,0,MAX_BUF_LEN);
	memset(unCompressBuf,0,MAX_BUF_LEN);


	if (len != sizeof(control_header)){

		CLog::Log(LOG_LEVEL_WARNING,"Input Len is Not Correct.\n");
		return -1;
	
	}

	if (strncmp((char *)pCtlHeader->magic,(char *)BUFF_TAG,5) != 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Buffer Tag is Not Correct.\n");
		return -2;
	}
	
	bCmd = pCtlHeader->cmd;
	uOrgLen = pCtlHeader->dataLen;
	uCompLen = pCtlHeader->compressLen;
	
	if (uOrgLen > 0 ){
			
	/*	nRet = recv(cliSocket,(char *)recvBuf,MAX_BUF_LEN,0);
		if(nRet == 0 || nRet == SOCKET_ERROR){

			CLog::Log(LOG_LEVEL_WARNING,"Client %d Quit.\n",cliSocket);
			return -3;
			
		}else{
			
			CLog::Log(LOG_LEVEL_WARNING,"Recv Data Len :%d.\n",nRet);

		}
		*/
		nRet = RecvDataFromPeer(pClient,recvBuf,uCompLen);
		if (nRet < 0 ){

			CLog::Log(LOG_LEVEL_WARNING,"Client %d Quit.\n",cliSocket);
			return -3;

		}
		
		uOrgLen = MAX_BUF_LEN;
		//解压缩
		nRet = uncompress(unCompressBuf,&uOrgLen,recvBuf,uCompLen);
		if (nRet != 0 ){
			
			CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
			return -4;

		}

	}

	//处理请求报文，生成应答报文
	nRet = 0;
	nRet = process_recv_data(pClient,unCompressBuf,uOrgLen,bCmd);
	if (nRet != 0){
		
		CLog::Log(LOG_LEVEL_WARNING,"Process Recv Data Error\n");
		return -4;

	}
	
	//CLog::Log(LOG_LEVEL_WARNING,"Process Recv Data OK.\n");

	return 0;
}


//处理请求，生成应答并发送

INT process_recv_data(LPVOID pClient, LPBYTE pData, UINT len,BYTE cmdTag){

	//BYTE bCmd = pData[0];   //获得请求命令字
	
	INT nRet = 0;
	

	nRet = doRecvData(pClient,pData,len,cmdTag);
	if (nRet != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Recv Data Done Error\n");
		return -1;
	}


	CLog::Log(LOG_LEVEL_WARNING,"Recv Data Done OK\n");

	return 0;
}




