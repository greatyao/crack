
#include "PacketUtils.h"

#include "zlib.h"
#include "zconf.h"


extern unsigned int g_file_len;
extern FILE *g_upload_file;

extern SOCKET g_socket;

int genLoginPacket(unsigned char *pdata, int *len){

	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(TOKEN_LOGIN);

	
	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	*len = CTL_HDR_LEN;

	return ret;

}

int genKeeplivePacket(unsigned char *pdata, int *len){

	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(TOKEN_HEARTBEAT);
	
	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	*len = CTL_HDR_LEN;
	return ret;

}

/*
unsigned char algo;		//解密算法
	unsigned char charset;	//解密字符集
	unsigned char type;		//解密类型
	unsigned char special;	//是否是文件解密（pdf+office+rar+zip）
	unsigned char startLength;//起始长度
	unsigned char endLength;	//终结长度
	unsigned char filename[256];	//用户传过来的文件名
	*/

int genUploadTaskPacket(unsigned char *pdata, int *len){

	unsigned char compBuf[MAX_BUF_LEN];

	int ret = 0;
	crack_task ctask;
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;

	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_UPLOAD);
	
	ctask.algo = algo_md4;
	ctask.charset = charset_num;
	
	ctask.type = bruteforce;
	ctask.special = 0;

	ctask.startLength = 3;
	ctask.endLength = 10;
	memcpy(ctask.filename,"01234567890123456",16);
	
	memset(compBuf,0,MAX_BUF_LEN);

	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(crack_task);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&ctask,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;

	return ret;
}

int genStartTaskPacket(unsigned char *pdata, int *len){

	int ret = 0;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;

	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_START);
	task_start_req startreq;

	memcpy(startreq.guid,"0123456789012345678901234567890123456789",40);

	memset(compBuf,0,MAX_BUF_LEN);

	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(task_start_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&startreq,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;
	
	return ret;
}



int genStopTaskPacket(unsigned char *pdata, int *len){

	int ret = 0;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;

	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_STOP);
	task_stop_req stopreq;

	memcpy(stopreq.guid,"1111111111012345678901234567890123456789",40);

	memset(compBuf,0,MAX_BUF_LEN);

	lCompressLen = MAX_BUF_LEN;
	
	lUnCompressLen = sizeof(task_stop_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&stopreq,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;
	
	return ret;


}

int genDeleteTaskPacket(unsigned char *pdata, int *len){


	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_DELETE);
	int ret = 0;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;

	task_delete_req delreq;

	memcpy(delreq.guid,"2222222222012345678901234567890123456789",40);

	memset(compBuf,0,MAX_BUF_LEN);

	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(task_delete_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&delreq,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;
	
	return ret;
}



int genPauseTaskPacket(unsigned char *pdata, int *len){


	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_PAUSE);
	int ret = 0;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;
	task_pause_req pausereq;

	memcpy(pausereq.guid,"3333333333012345678901234567890123456789",40);

	memset(compBuf,0,MAX_BUF_LEN);

	
	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(task_pause_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&pausereq,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;
	
	return ret;
}

int genGetTaskResPacket(unsigned char *pdata, int *len){

	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_TASK_RESULT);
	task_result_req resultreq;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;

	memcpy(resultreq.guid,"4444444444012345678901234567890123456789",40);

	memset(compBuf,0,MAX_BUF_LEN);

	
	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(task_result_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&resultreq,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;
	
	return ret;
	
}



int genTaskStatusPacket(unsigned char *pdata, int *len){

	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_REFRESH_STATUS);
	
	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	*len = CTL_HDR_LEN;


	return ret;

}

int genClientListPacket(unsigned char *pdata, int *len){


	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_GET_CLIENT_LIST);

	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	*len = CTL_HDR_LEN;
	return ret;

}


//upload file
//client ---> server 
// upload req : 
// data : guid
int genTaskFileUpload(unsigned char *pdata, int *len){

	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_UPLOAD_FILE);
	file_upload_req fileupload;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;

	
	memcpy(fileupload.guid,"aaa",3);

	memset(compBuf,0,MAX_BUF_LEN);

	
	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(file_upload_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&fileupload,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;
	
	return ret;
	
}


//transport file upload start req
//client ---> server
//upload start req :
// data : guid | NULL | file len | file offset 
int genTaskFileUploadStart(unsigned char *pdata, int *len){

	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_START_UPLOAD);
	file_upload_start_req uploadstartreq;
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;
	unsigned int filelen = 0;
	unsigned int readLen = 0;
	FILE *fp = NULL;

	//file read 

	memset(compBuf,0,MAX_BUF_LEN);

	memset(uploadstartreq.guid,0,40);

	memcpy(uploadstartreq.guid,"aaa",4);

	fp = fopen((char *)uploadstartreq.guid,"rb");
	if (!fp){
		

		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s Error\n",uploadstartreq.guid);
		return -1;

	}

	fseek(fp,0L,SEEK_END);

	filelen = ftell(fp);

	fseek(fp,0L,SEEK_SET);


	g_file_len = filelen;

	uploadstartreq.len = filelen;
	uploadstartreq.f = NULL;
	
	g_upload_file = fp;

	lCompressLen = MAX_BUF_LEN;
	lUnCompressLen = sizeof(file_upload_start_req);
	
	ret = compress(compBuf,&lCompressLen,(unsigned char *)&uploadstartreq,lUnCompressLen);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Compress Send Buff Error\n");
		return -1;

	}
	

	clthdr.dataLen = lUnCompressLen;
	clthdr.compressLen = lCompressLen;


	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,compBuf,lCompressLen);
	*len = CTL_HDR_LEN + lCompressLen;

	return ret;

}


//发送数据函数，for file transfor
int transforUploading(SOCKET sock,unsigned char *psend,int sendlen){

	int ret = 0;
	control_header *pctlheader = (control_header *)psend;

	printf("Send CMD is : %d,data len is : %d,compress len : %d\n",pctlheader->cmd,pctlheader->dataLen,pctlheader->compressLen);

	ret = send(sock,(char *)psend,CTL_HDR_LEN,0);
	if (ret == SOCKET_ERROR){

		printf("Send Header Error %s\n",GetLastError());
		return -1;
	}


	if (sendlen > CTL_HDR_LEN){


		ret = send(sock,(char *)psend+CTL_HDR_LEN,sendlen-CTL_HDR_LEN,0);
		if (ret == SOCKET_ERROR){

			printf("Send Info Error\n");
			return -2;

		}


	}

	return ret;
}



//transfor file 
//date :  file data (plain data)
int genTaskFileUploading(unsigned char *pdata, int *len){
	
	int ret = 0;
	control_header clthdr = INITIALIZE_EMPTY_HEADER(CMD_START_UPLOAD);
	unsigned char compBuf[MAX_BUF_LEN];
	unsigned long lCompressLen= 0;
	unsigned long lUnCompressLen = 0;
	unsigned int filelen = 0;
	unsigned int readLen = 0;
	unsigned char sendbuf[MAX_BUF_LEN];
	FILE *fp = NULL;

	//file read 
	
	fp = g_upload_file;
	while(!feof(fp)){
		memset(sendbuf,0,MAX_BUF_LEN);
		readLen = fread(sendbuf+sizeof(control_header),1,1024,fp);
		if (readLen < 0 ){
			CLog::Log(LOG_LEVEL_WARNING,"read file Error\n");
			return -1;
		}

		
		clthdr.dataLen = readLen;
		clthdr.compressLen = -1;

		memcpy(sendbuf,(unsigned char *)&clthdr,sizeof(control_header));

		//send file buffer 
		ret = transforUploading(g_socket,sendbuf,CTL_HDR_LEN+readLen);
		if(ret < 0 ){

			printf("Send file buffer error\n");
			break;
		}

		printf("Send file buffer %d vs %d ok\n",ret,g_file_len);
		//
	}

	memcpy(pdata,&clthdr,CTL_HDR_LEN);
	memcpy(pdata+CTL_HDR_LEN,sendbuf,readLen);
	*len = CTL_HDR_LEN + readLen;

	return ret;

}





