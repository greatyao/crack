
#include "PacketUtils.h"

#include "zlib.h"
#include "zconf.h"




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






