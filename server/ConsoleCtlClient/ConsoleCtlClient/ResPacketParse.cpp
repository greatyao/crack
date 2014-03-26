
#include "ResPacketParse.h"
#include "zlib.h"
#include "zconf.h"


void getLoginResPacket(unsigned char *pdata, int len){

	

	printf("Enter into login res parse,%d\n",len);
	return ;


}

void getKeepliveResPacket(unsigned char *pdata, int len){

	printf("Enter into keeplive res packet %d\n",len);
	

	return;
}


void getUploadTaskResPacket(unsigned char *pdata, int len){

	
	printf("Enter into Upload task packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	task_upload_res *pUploadRes = NULL;

	memset(uncompbuf,0,RES_BUF_LEN);
	
	luncompresslen = RES_BUF_LEN;
	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pUploadRes = (task_upload_res *)uncompbuf;
	printf("task upload res : %s\n",pUploadRes->guid);

	return ;
}

void getStartTaskResPacket(unsigned char *pdata, int len){


	printf("Enter into Start task packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	task_status_res *pRes = NULL;

	memset(uncompbuf,0,RES_BUF_LEN);
	
	luncompresslen = RES_BUF_LEN;
	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pRes = (task_status_res *)uncompbuf;
	printf("task start res guid : %s, status : %d\n",pRes->guid,pRes->status);

	return ;


}



void getStopTaskResPacket(unsigned char *pdata, int len){

	printf("Enter into Stop task packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	task_status_res *pRes = NULL;

	memset(uncompbuf,0,RES_BUF_LEN);
	
	luncompresslen = RES_BUF_LEN;

	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pRes = (task_status_res *)uncompbuf;
	printf("task Stop res guid : %s,status : %d\n",pRes->guid,pRes->status);

	return ;
}

void getDeleteTaskResPacket(unsigned char *pdata, int len){


	printf("Enter into Delete task packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	task_status_res *pRes = NULL;

	luncompresslen = RES_BUF_LEN;
	memset(uncompbuf,0,RES_BUF_LEN);
	
	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pRes = (task_status_res *)uncompbuf;
	printf("task Delete res guid : %s, status : %d\n",pRes->guid,pRes->status);

	return ;


}



void getPauseTaskResPacket(unsigned char *pdata, int len){


	printf("Enter into Pause task packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	task_status_res *pRes = NULL;

	memset(uncompbuf,0,RES_BUF_LEN);
	luncompresslen = RES_BUF_LEN;
	
	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pRes = (task_status_res *)uncompbuf;
	printf("task Pause res guid : %s, status : %d\n",pRes->guid,pRes->status);

	return ;


}

void getGetTaskResPacket(unsigned char *pdata, int len){

	

	printf("Enter into Get task Result packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	task_status_res *pRes = NULL;

	luncompresslen = RES_BUF_LEN;
	memset(uncompbuf,0,RES_BUF_LEN);
	
	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pRes = (task_status_res *)uncompbuf;
	printf("task Task result guid : %s, status : %d,password : %s\n",pRes->guid,pRes->status,pRes->password);

	return ;


}



void getTaskStatusResPacket(unsigned char *pdata, int len){

	printf("Enter into Get task stauts info packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	int num = 0;
	int i = 0;
	task_status_info *pStatusInfo = NULL;
	task_status_info *pCurInfo = NULL;

	memset(uncompbuf,0,RES_BUF_LEN);
	
	luncompresslen = RES_BUF_LEN;

	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pStatusInfo = (task_status_info *)uncompbuf;
	
	num = luncompresslen /sizeof(task_status_info);
	for (i = 0 ; i < num ;i ++ ){

		
		pCurInfo = pStatusInfo + i;
		printf("task Task status info guid : %s, status : %d,process : %f\n",pCurInfo->guid,pCurInfo->status,pCurInfo->m_progress);

		

	}
	

	//printf("task Task status info guid : %s, status : %d,password : %s\n",pRes->guid,pRes->status,pRes->password);

	return ;

}

void getClientListResPacket(unsigned char *pdata, int len){

	
	printf("Enter into Get task stauts info packet %d\n",len);
	int ret = 0;
	unsigned long lcompresslen =  len;
	unsigned long luncompresslen = 0;
	unsigned char uncompbuf[RES_BUF_LEN];
	int num = 0;
	int i = 0;
	compute_node_info *pComputeNodes = NULL;
	compute_node_info *pCurNode = NULL;

	memset(uncompbuf,0,RES_BUF_LEN);
	
	luncompresslen = RES_BUF_LEN;

	//uncompress
	ret = uncompress(uncompbuf,&luncompresslen,pdata,lcompresslen);
	if (ret != 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"UnCompress Error\n");
		return ;

	}

	pComputeNodes = (compute_node_info *)uncompbuf;
	
	num = luncompresslen /sizeof(compute_node_info);
	for (i = 0 ; i < num ;i ++ ){

		
		pCurNode = pComputeNodes + i;
		printf("Task Compute Node info guid : %s,hostname : %s,ip : %s\n",pCurNode->guid,pCurNode->hostname,pCurNode->ip);
	

	}
	

	//printf("task Task status info guid : %s, status : %d,password : %s\n",pRes->guid,pRes->status,pRes->password);

	return ;


}




