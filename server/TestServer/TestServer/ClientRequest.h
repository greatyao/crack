
#ifndef _CLIENT_REQUEST_H_
#define _CLIENT_REQUEST_H_

#include <string>

//�ͻ����������ݽṹ

//�����ϴ�����
typedef struct _CTL_CLIENT_TASK_UPLOAD_{

	std::string m_algorithm;
	std::string m_hashinfo;
	std::string m_ctlguid;


}CtlTaskUpload;


//����ʼ���󣬷�������Ӧ������󣬶�������л���
typedef struct _CTL_CLIENT_TASK_START_{

	std::string m_guid;
	unsigned char m_priority;
	std::string m_start;
	std::string m_end;
	std::string m_charset;


}CtlTaskStart;

//����ֹͣ����
typedef struct _CTL_CLIENT_TASK_STOP_{

	std::string m_guid;

}CtlTaskStop;

//����ɾ������
typedef struct _CTL_CLIENT_TASK_DEL_{

	std::string m_guid;

}CtlTaskDel;


//������ͣ����
typedef struct _CTL_CLIENT_TASK_PAUSE_{

	std::string m_guid;

}CtlTaskPause;


//�õ�������
typedef struct _CTL_CLIENT_TASK_GUID_{

	std::string m_guid;
}CtlTaskGuid;



//�õ�����ִ��������Ϣ
//��


//�õ��ͻ����б�

//��

//����ڵ��������
//�õ��������������,���ܽ������

typedef struct _COMP_CLIENT_WORKITEM_RECOVERED_{

	unsigned char m_wi_status;	//�������״̬
	std::string m_result;
	std::string m_guid;


}CompWIRecovered;

//�õ���������ɣ���δ���ܳ����
typedef struct _COMP_CLIENT_WORKITEM_UNRECOVERED_{

	unsigned char m_wi_status;	
	std::string m_guid;

}CompWIUnRecovered;


//����ַ��¹�����
typedef struct _COMP_CLIENT_GUID_{

	std::string m_guid;

}CompGuid;

//���������ʧ��֪ͨ
typedef struct _COMP_CLIENT_WORKITEM_FAILRED_{

	std::string m_wi_guid;
	std::string m_comp_guid;

}CompWIFailed;



//////////////�ļ��ϴ�������///////////////////////


//�������ļ��ϴ���ʼ
typedef struct _CLIENT_UPLOAD_FILE_START_{

	unsigned char filename[256];
	unsigned int filelen;
	unsigned int blocknum;
	unsigned int blocksize;
	
}CltUploadFileStart;

typedef struct _CLIENT_UPLOAD_FILE_BLOCK_INFO_{

	unsigned char filename[256];
	unsigned int currentid;
	unsigned int currentlen;
	//unsigned char *pdata;

}CltUploadFileBlockInfo;

typedef struct _CLIENT_UPLOAD_FILE_END_{

	unsigned char filename[256];
	unsigned int filelen;
	unsigned char end;			//�ϴ�������־ 1����

}CltUploadFileEnd;



//�������ļ����ؿ�ʼ
typedef struct _CLIENT_DOWNLOAD_FILE_START_{

	unsigned char filename[256];
	
}CltDownloadFileStart;

typedef struct _CLIENT_DOWNLOAD_FILE_BLOCK_INFO_{

	unsigned char filename[256];
	unsigned int currentlen;
	unsigned int datalen;
	//unsigned char *pdata;

}CltDownloadFileBlockInfo;

typedef struct _CLIENT_DOWNLOAD_FILE_END_{

	unsigned char filename[256];
	unsigned char end;			//�ϴ�������־ 1����

}CltDownloadFileEnd;


#endif 
