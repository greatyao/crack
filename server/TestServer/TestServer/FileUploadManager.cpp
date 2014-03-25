#include "StdAfx.h"
#include "FileUploadManager.h"
#include "CLog.h"


CFileUploadManager::CFileUploadManager(void)
{
	BOOL bCreate = FALSE;
	int i = 0;
	ZeroMemory(m_UploadFileInfo,sizeof(FileTranInfo)*100);
	
	bCreate = CreateDirectoryA(UPLOAD_FILE_DIR,NULL);
	if (!bCreate){

		CLog::Log(LOG_LEVEL_WARNING,"Create Directory Error\n");
		
	}

	CLog::Log(LOG_LEVEL_WARNING,"File Upload Manager Init OK\n");
}

CFileUploadManager::~CFileUploadManager(void)
{
}
/*

unsigned char filename[256];	//文件名称
	unsigned int filelen;			//文件长度
	unsigned int blocknum;			//文件分块数
	unsigned int blocksize;			//分块大小
	unsigned int currentid;			//当前分块id
	unsigned int curlen;			//当前传输文件长度
	
	unsigned char used;

	*/

FileTranInfo *CFileUploadManager::findFreeNode(){
	
	FileTranInfo *pFileInfo = NULL;
	int i = 0;

	for(i = 0; i < 100;i ++){

		pFileInfo = &m_UploadFileInfo[i];
		if (pFileInfo->used == 0){
			return pFileInfo;

		}
			
	}

	if (i == 100){

		pFileInfo = NULL;
	}

	return pFileInfo;
}



FileTranInfo *CFileUploadManager::findFileByFilename(unsigned char *pfilename){


	FileTranInfo *pFileInfo = NULL;

	int i = 0;

	for(i = 0;i < 100 ;i ++ ){


		pFileInfo = &m_UploadFileInfo[i];

		if (strncmp((char *)pFileInfo->filename,(char *)pfilename,strlen((char *)pfilename)) == 0){
			
			return pFileInfo;

		}

	}

	if (i == 100){

		pFileInfo = NULL;
	}

	return pFileInfo;

}

//开始请求
INT CFileUploadManager::UploadStart(CltUploadFileStart *pFileStart){

	
	int ret = 0;
	FILE *pfile = NULL;
	char filewithpath[1024];
	FileTranInfo *pFileInfo = findFreeNode();
	if (pFileInfo == NULL){

		CLog::Log(LOG_LEVEL_WARNING,"Find Free File Info Error\n");
		return -1;  //未找到有效资源
	}
		
	CopyMemory(pFileInfo->filename,pFileStart->filename,strlen((char *)pFileStart->filename));
	pFileInfo->filelen = pFileStart->filelen;
	pFileInfo->blocknum = pFileStart->blocknum;
	pFileInfo->blocksize = pFileStart->blocksize;
	
	pFileInfo->used = 1;
	pFileInfo->curlen = 0;
	pFileInfo->currentid = 0;

	ZeroMemory(filewithpath,1024);
	CopyMemory(filewithpath,UPLOAD_FILE_DIR,strlen(UPLOAD_FILE_DIR));
	strcat(filewithpath,"\\");
	strcat(filewithpath,(char *)pFileStart->filename);

	//创建文件
	pfile = fopen(filewithpath,"wb");
	if (!pfile){
		CLog::Log(LOG_LEVEL_WARNING,"Create file Error\n");
		return -2;
	}
	fclose(pfile);
	return 0;
}

	//文件传输请求
INT CFileUploadManager::UploadTransport(CltUploadFileBlockInfo *pFileInfo,unsigned char *pdata){

	FILE *pfile = NULL; 
	FileTranInfo *pFileTran = NULL;
	char filewithpath[1024];
	pFileTran = findFileByFilename(pFileInfo->filename);
	if (!pFileTran){

		CLog::Log(LOG_LEVEL_WARNING,"find file trans file Error\n");
		return -2;
			
	}

	ZeroMemory(filewithpath,1024);
	CopyMemory(filewithpath,UPLOAD_FILE_DIR,strlen(UPLOAD_FILE_DIR));
	strcat(filewithpath,"\\");
	strcat(filewithpath,(char *)pFileInfo->filename);

	pFileTran->curlen += pFileInfo->currentlen;
	if ((pFileTran->currentid +1) == pFileInfo->currentid){
		pFileTran->currentid = pFileInfo->currentid;
	}else{

		CLog::Log(LOG_LEVEL_WARNING,"Tran file info is disorder\n");
		return -3;

	}
	
	//将信息写入文件
	pfile = fopen(filewithpath,"ab");
	if(!pfile){
		
		CLog::Log(LOG_LEVEL_WARNING,"Fopen file write upload info Error\n");
		return -3;

	}

	fwrite(pdata,1,pFileInfo->currentlen,pfile);

	fclose(pfile);

	return 0;
}

	//传输结束请求
INT CFileUploadManager::UploadEnd(CltUploadFileEnd *pFileEnd){

	FILE *pfile = NULL; 
	FileTranInfo *pFileTran = NULL;
	char filewithpath[1024];
	pFileTran = findFileByFilename(pFileEnd->filename);
	if (!pFileTran){

		CLog::Log(LOG_LEVEL_WARNING,"find file trans file Error\n");
		return -2;
			
	}

	ZeroMemory(filewithpath,1024);
	CopyMemory(filewithpath,UPLOAD_FILE_DIR,strlen(UPLOAD_FILE_DIR));
	strcat(filewithpath,"\\");
	strcat(filewithpath,(char *)pFileEnd->filename);

	//判断文件是否完整
	if (pFileTran->curlen != pFileEnd->filelen){
			
		CLog::Log(LOG_LEVEL_WARNING,"File trans file Error\n");
		return -3;

	}

	if (pFileEnd->end == 0){

		CLog::Log(LOG_LEVEL_WARNING,"File End Struct Error\n");
		return -4;
	}

	ZeroMemory(pFileTran,sizeof(FileTranInfo));

	return 0;
}



//开始下载请求
CltUploadFileStart *CFileUploadManager::DownLoadStart(CltDownloadFileStart *pFileStart){

	int ret = 0;
	unsigned int filelen = 0;
	unsigned int blocknum = 0;
	int it = 0;
	CltUploadFileStart *pUploadStart = NULL;

	FILE *pfile = NULL;
	char filewithpath[1024];
	

	ZeroMemory(filewithpath,1024);
	CopyMemory(filewithpath,UPLOAD_FILE_DIR,strlen(UPLOAD_FILE_DIR));
	strcat(filewithpath,"\\");
	strcat(filewithpath,(char *)pFileStart->filename);

	//读取文件基本信息
	pfile = fopen(filewithpath,"rb");
	if (!pfile){
		CLog::Log(LOG_LEVEL_WARNING,"Read file Error\n");
		return NULL;
	}

	fseek(pfile,0L,SEEK_END);
	filelen = ftell(pfile);  //获取文件长度
	
	fclose(pfile);

	blocknum = filelen/BLOCK_SIZE;
	it = filelen % BLOCK_SIZE;
	if (it > 0){
		blocknum = blocknum+1;
	}

	/*
		unsigned char filename[256];
	unsigned int filelen;
	unsigned int blocknum;
	unsigned int blocksize;
	*/

	pUploadStart =(CltUploadFileStart*)_Alloc(sizeof(CltUploadFileStart));

	if (!pUploadStart){
			
		CLog::Log(LOG_LEVEL_WARNING,"Alloc Error\n");
		return NULL;

	}



	CopyMemory(pUploadStart->filename,pFileStart->filename,sizeof(pFileStart->filename));
	pUploadStart->filelen = filelen;
	pUploadStart->blocknum = blocknum;
	pUploadStart->blocksize = BLOCK_SIZE;
	return pUploadStart;
}


//下载请求
unsigned char * CFileUploadManager::DownLoadTransport(CltDownloadFileBlockInfo *pFileBlock){

	int ret = 0;
	FILE *pfile = NULL;
	char filewithpath[1024];
	unsigned char * pdata = NULL;
	/*
		unsigned char filename[256];
	unsigned int currentlen;
	unsigned int datalen;
	*/
	
	
	ZeroMemory(filewithpath,1024);
	CopyMemory(filewithpath,UPLOAD_FILE_DIR,strlen(UPLOAD_FILE_DIR));
	strcat(filewithpath,"\\");
	strcat(filewithpath,(char *)pFileBlock->filename);

	pdata = (unsigned char *)_Alloc(pFileBlock->datalen);
	if (!pdata){

		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s Error\n",filewithpath);
		return NULL;
	}

	pfile = fopen(filewithpath,"rb");
	if (!pfile){

		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s Error\n",filewithpath);
		_Free(pdata);
		return NULL;
	}

	ret = fseek(pfile,pFileBlock->currentlen,SEEK_SET);
	if (ret != 0){
		CLog::Log(LOG_LEVEL_WARNING,"File position %s Error\n",filewithpath);
		fclose(pfile);
		_Free(pdata);
		return NULL;


	}
	
	fread(pdata,1,BLOCK_SIZE,pfile);
	fclose(pfile);

	return pdata;

}

//完成下载请求
/*
INT CFileUploadManager::DownloadEnd(CltDownloadFileEnd *pFileEnd){

	int ret = 0;
	





	return ret;

}

*/

void * CFileUploadManager::_Alloc(int size){

	
	unsigned char * p = NULL;

	p = (unsigned char *)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);
	
	return (void *)p;

}

void CFileUploadManager::_Free(void * pt){

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pt);

}





