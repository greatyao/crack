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

	//��ȡ���нڵ�
	//FileTranInfo * GetFreeFileNode();

	//��ʼ����
	INT UploadStart(CltUploadFileStart *pFileStart);

	//�ļ���������
	INT UploadTransport(CltUploadFileBlockInfo *pFileInfo,unsigned char *pdata);

	//�����������
	INT UploadEnd(CltUploadFileEnd *pFileEnd);



	//��ʼ��������
	CltUploadFileStart *DownLoadStart(CltDownloadFileStart *pFileStart);


	//��������
	unsigned char *DownLoadTransport(CltDownloadFileBlockInfo *pFileStart);

	//�����������
//	INT DownloadEnd(CltDownloadFileEnd *pFileEnd);


private:

	FileTranInfo *findFreeNode();
	
	FileTranInfo *findFileByFilename(unsigned char *pfilename);

	void * _Alloc(int size);

	void _Free(void *pVoid);

	
};
