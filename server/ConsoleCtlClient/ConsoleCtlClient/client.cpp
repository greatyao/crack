#include <iostream>
#include <Windows.h>

#include "PacketUtils.h"
#include "ResPacketParse.h"

#include "CLog.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

#define MAX_LEN 1500
#define PORT 6010
#define IP_ADDRESS "127.0.0.1"



void doRecvData(int tag,unsigned char *pdata,int len){

	int ret = 0;

	switch(tag){


		case TOKEN_LOGIN:

			getLoginResPacket(pdata,len);

			break;
		case TOKEN_HEARTBEAT:
			getKeepliveResPacket(pdata,len);

			break;
		case CMD_TASK_UPLOAD:
			getUploadTaskResPacket(pdata,len);

			break;
			/*
			CMD_TASK_START,		//开始任务
	CMD_TASK_STOP,		//停止任务
	CMD_TASK_DELETE,	//删除任务
	CMD_TASK_PAUSE,		//暂停任务
	CMD_TASK_RESULT,	//取得解密信息（解密状态）

	CMD_REFRESH_STATUS,	//取得任务的进度和状态等信息
	CMD_GET_CLIENT_LIST,//返回在线计算机信息的列表
	*/
		case CMD_TASK_START:
			getStartTaskResPacket(pdata,len);

			break;
		case CMD_TASK_STOP:

			getStopTaskResPacket(pdata,len);
			break;
		case CMD_TASK_DELETE:

			getDeleteTaskResPacket(pdata,len);

			break;



		case CMD_TASK_PAUSE:

			getPauseTaskResPacket(pdata,len);
			break;

		case CMD_TASK_RESULT:
			getGetTaskResPacket(pdata,len);

			break;
		case CMD_REFRESH_STATUS:

			getTaskStatusResPacket(pdata,len);

			break;



		case CMD_GET_CLIENT_LIST:

			getClientListResPacket(pdata,len);
			break;

		default :

			printf("Not Support this Num %d\n",tag);

			break;

	}

	return ;
}

int getSendBuf(int tag ,unsigned char *pdata, int *len){


	int ret = 0;
	switch(tag){


		case 0:

			genLoginPacket(pdata,len);

			break;
		case 1:
			genKeeplivePacket(pdata,len);


			break;
		case 2:
			genUploadTaskPacket(pdata,len);


			break;


		case 3:
			genStartTaskPacket(pdata,len);

			break;

		case 4:

			genStopTaskPacket(pdata,len);
			break;
		case 5:

			genDeleteTaskPacket(pdata,len);

			break;



		case 6:

			genPauseTaskPacket(pdata,len);
			break;

		case 7:
			genGetTaskResPacket(pdata,len);

			break;
		case 8:

			genTaskStatusPacket(pdata,len);

			break;



		case 9:

			genClientListPacket(pdata,len);
			break;

		case 10:

			printf("You select Exit\n");

			ret = -2;
			break;
		default :

			printf("You select Error num\n");
			ret = -1;
			break;

	}


	return ret;

}


int selectFun(){

	int ret = 0;

	printf("0 Login Server\n");
	printf("1 HeartBeat Server\n");
	printf("2 Upload Task\n");
	printf("3 Start Task,include split task to workitems\n");

	printf("4 Stop Task\n");
	printf("5 Delete Task\n");
	printf("6 Pause Task\n");

	printf("7 Get A Task Result\n");

	printf("8 Get Tasks Status\n");
	printf("9 Get Computing Clients Status\n");
	printf("10 exit\n");


	scanf("%d",&ret);
	printf("You select %d\n",ret);


	return ret;
}



int SendDataToServer(SOCKET sock,unsigned char *psend,int sendlen){

	int ret = 0;
	control_header *pctlheader = (control_header *)psend;

	printf("Send CMD is : %d,data len is : %d,compress len : %d\n",pctlheader->cmd,pctlheader->dataLen,pctlheader->compressLen);

	ret = send(sock,(char *)psend,CTL_HDR_LEN,0);
	if (ret == SOCKET_ERROR){

		cout<<"Send Header Error::"<<GetLastError()<<endl;
		return -1;
	}


	if (sendlen > CTL_HDR_LEN){


		ret = send(sock,(char *)psend+CTL_HDR_LEN,sendlen-CTL_HDR_LEN,0);
		if (ret == SOCKET_ERROR){

			cout<<"Send Info Error::"<<GetLastError()<<endl;
			return -2;

		}


	}

	return 0;
}


int RecvServerData(SOCKET sock){

	int ret = 0;
	control_header reshdr;
	unsigned int icompresslen = 0;
	unsigned int iuncompresslen = 0;
	unsigned char recvBuf[MAX_LEN];


	ret = recv(sock,(char *)&reshdr,CTL_HDR_LEN,0);
	if(ret == 0 || ret == SOCKET_ERROR){

		cout <<"Client Recv Control Header Error!"<<endl;
		return -1;
	}

	icompresslen = reshdr.compressLen;
	iuncompresslen = reshdr.dataLen;

	if (icompresslen == 0 ){

		printf("Recv Res Data %d,Res status :%d \n",reshdr.cmd,reshdr.response);

		return 0;
	}


	memset(recvBuf,0,MAX_LEN);
	ret = 0;

	ret = recv(sock,(char *)recvBuf,icompresslen,0);
	if(ret == 0 || ret == SOCKET_ERROR){

		cout <<"Client Recv Error!"<<endl;
		return -1;
	}


	doRecvData(reshdr.cmd,recvBuf,icompresslen);

	return ret;

}


void RecvServerData(void *pClient,char * pData,int size){


	int Ret = 0;
	Ret = recv(*(SOCKET*)pClient,pData,size,0);
	if(Ret == 0 || Ret == SOCKET_ERROR){

		cout <<"Client Quit!"<<endl;
		return ;
	}

	cout <<"Client Recv !"<< pData<<endl;

}


int main(int argc,char *argv[]){

	WSADATA ws;
	SOCKET ClientSocket;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;
	char SendBuffer[MAX_LEN];
	char RecvBuffer[MAX_LEN];
	int sendlen = 0;
	int recvLen = 0;
	int ifun = 0;

	
	CLog::InitLogSystem(LOG_TO_FILE,TRUE,"E:\\DecSys\\Client.log");

	if (WSAStartup(MAKEWORD(2,2),&ws)!= 0){

		cout<<"Init Windows Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}

	ClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET){

		cout<<"Create Socket Failed::"<<GetLastError()<<endl;
		return -1;

	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	ServerAddr.sin_port = htons(PORT);
	memset(ServerAddr.sin_zero,0x00,8);

	/*	ifun = selectFun();

	printf("ifun is : %d\n",ifun);
	*/

	Ret = connect(ClientSocket,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr));
	if (Ret == SOCKET_ERROR){

		cout<<"Connect Failed::"<<GetLastError()<<endl;
		return -1;

	}else{

		cout<<"Connect Success!"<<endl;

	}


	while(true){


		ifun = selectFun();

		printf("You Select : %d\n",ifun);

		memset(SendBuffer,0,MAX_LEN);
		Ret = getSendBuf(ifun,(unsigned char *)SendBuffer,&sendlen);
		if (Ret == -2 ){

			printf("You Select Exit\n");
			break;
		}
		

		//cin.getline(SendBuffer,sizeof(SendBuffer));
		while (sendlen == 0){

			cout<<"Input size should not be 0"<<endl;

			ifun = selectFun();

			Ret = getSendBuf(ifun,(unsigned char *)SendBuffer,&sendlen);
			if (Ret == -2 ){

				printf("You Select Exit\n");
				closesocket(ClientSocket);
				WSACleanup();
				return -1;
			}
		}



		Ret = SendDataToServer(ClientSocket,(unsigned char *)SendBuffer,sendlen);
		if (Ret < 0 ){

			printf("Send Data Error\n");
			return -2;

		}

		/*	Ret = send(ClientSocket,SendBuffer,(int)strlen(SendBuffer),0);
		if (Ret == SOCKET_ERROR){

		cout<<"Send Info Error::"<<GetLastError()<<endl;
		return -1;
		}

		*/

		memset(RecvBuffer,0,MAX_LEN);
		recvLen  = MAX_LEN;
		//	RecvServerData(&ClientSocket,RecvBuffer,recvLen);
		RecvServerData(ClientSocket);


	}


	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}