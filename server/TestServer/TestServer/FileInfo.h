
#ifndef _FILE_INFO_H_
#define _FILE_INFO_H_

typedef struct _FILE_TRANS_INFO_ {
		
	unsigned char filename[256];	//�ļ�����
	unsigned int filelen;			//�ļ�����
	unsigned int blocknum;			//�ļ��ֿ���
	unsigned int blocksize;			//�ֿ��С
	unsigned int currentid;			//��ǰ�ֿ�id
	unsigned int curlen;			//��ǰ�����ļ�����
	
	unsigned char used;
//	_FILE_TRANS_INFO_ *pfileinfo;

}FileTranInfo;


//FileTranInfo g_TransFile[100];  //���ڴ�����ļ���Ϣ




#endif





