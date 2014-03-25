#pragma once

#include "FileInfo.h"
#include "ClientRequest.h"


#define UPLOAD_FILE_DIR "uploadfile"
#define BLOCK_SIZE 1024

class CFileUploadManager
{
public:
	FileTranInfo m_UploadFileInfo[100];

//	FileTranInfo m_downloadFileInfo[100];
//	int m_iFree = 0;

public:
	CFileUploadManager(void);
	~CFileUploadManager(void);

	//获取空闲节点
	//FileTranInfo * GetFreeFileNode();

	//开始请求
	INT UploadStart(CltUploadFileStart *pFileStart);

	//文件传输请求
	INT UploadTransport(CltUploadFileBlockInfo *pFileInfo,unsigned char *pdata);

	//传输结束请求
	INT UploadEnd(CltUploadFileEnd *pFileEnd);



	//开始下载请求
	CltUploadFileStart *DownLoadStart(CltDownloadFileStart *pFileStart);


	//下载请求
	unsigned char *DownLoadTransport(CltDownloadFileBlockInfo *pFileStart);

	//完成下载请求
//	INT DownloadEnd(CltDownloadFileEnd *pFileEnd);


private:

	FileTranInfo *findFreeNode();
	
	FileTranInfo *findFileByFilename(unsigned char *pfilename);

	void * _Alloc(int size);

	void _Free(void *pVoid);

	
};
