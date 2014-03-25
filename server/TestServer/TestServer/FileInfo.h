
#ifndef _FILE_INFO_H_
#define _FILE_INFO_H_

typedef struct _FILE_TRANS_INFO_ {
		
	unsigned char filename[256];	//文件名称
	unsigned int filelen;			//文件长度
	unsigned int blocknum;			//文件分块数
	unsigned int blocksize;			//分块大小
	unsigned int currentid;			//当前分块id
	unsigned int curlen;			//当前传输文件长度
	
	unsigned char used;
//	_FILE_TRANS_INFO_ *pfileinfo;

}FileTranInfo;


//FileTranInfo g_TransFile[100];  //正在传输的文件信息




#endif





