#pragma once

class CPacketManager
{
public:
	
//////�����������ݵ���ؽӿ�

	//���ƽڵ��ϴ���������ṹ
	virtual CtlTaskUpload * GetCtlTaskUploadReq(BYTE *pdata,int len) = 0;

	//���ƽڵ㿪ʼ�ļ��ϴ���������ṹ
	//���ƽڵ��ϴ��ļ�����ṹ
	//���ƽڵ�����ļ��ϴ�����ṹ
	virtual CltUploadFileStart * GetCtlTaskFileUploadStart(BYTE *pdata,int len) = 0;
	virtual CltUploadFileBlockInfo * GetCtlTaskFileUploadTran(BYTE *pdata,int len) = 0;
	virtual CltUploadFileEnd * GetCtlTaskFileUploadEnd(BYTE *pdata,int len) = 0;

	//���ƽڵ㿪ʼ��������ṹ
	virtual CtlTaskStart * GetCtlTaskStartReq(BYTE *pdata,int len) = 0;

	//���ƽ�ֹͣ��������ṹ
	virtual CtlTaskStop * GetCtlTaskStopReq(BYTE *pdata,int len) = 0;

	//���ƽ�ɾ����������ṹ
	virtual CtlTaskDel * GetCtlTaskDelReq(BYTE *pdata,int len) = 0;

	//���ƽڵ���ͣ��������ṹ
	virtual CtlTaskPause * GetCtlTaskPauseReq(BYTE *pdata,int len) = 0;
	
	//��ȡ����ִ�н������ṹ
	virtual CtlTaskGuid * GetCtlTaskResultReq(BYTE *pdata,int len) = 0;
	
	//��ȡ���ڽ�������״̬����ṹ
	//�޲���

	//��ȡ�������ڵ���Ϣ����ṹ
	//�޲���

	//�������ڵ�����
	//��ȡ���ܳɹ��������ṹ
	virtual CompWIRecovered * GetCompWIDecOKReq(BYTE *pdata,int len) = 0;

	//��ȡ������ɡ�δ�ҵ���������ṹ
	virtual CompWIUnRecovered * GetCompWIDecNoPwdReq(BYTE *pdata,int len) = 0;

	//��ȡ����ʧ������ṹ
	virtual WorkItemInfo * GetCompWIReq(BYTE *pdata,int len) = 0;

	//����ڵ������ļ�����
	virtual CltDownloadFileStart *GetCompDownloadStart(BYTE *pdata, int len) = 0;
	virtual CltDownloadFileBlockInfo *GetCompDownloadBlockInfo(BYTE *pdata, int len) = 0;
	virtual CltDownloadFileEnd *GetCompDownloadEnd(BYTE *pdata, int len) = 0;

	//��ȡ����������ṹ
	virtual CompWIFailed * GetCompWIFailedReq(BYTE *pdata,int len) = 0;

//////����Ӧ�����ݵ���ؽӿ�

	//���������ϴ�Ӧ������
//	virtual LPBYTE GetTaskUploadRes()=0;

	//��������ʼӦ������
//	virtual LPBYTE GetTaskStartRes()=0;

	//��������ֹͣӦ������
//	virtual LPBYTE GetTaskUploadRes()=0;

	//��������ɾ��Ӧ������
//	virtual LPBYTE GetTaskStartRes()=0;

	//����������ͣӦ������
//	virtual LPBYTE GetTaskUploadRes()=0;

	//������ȡ������Ӧ������
//	virtual LPBYTE GetTaskStartRes()=0;

	//������ȡ����״̬Ӧ������
//	virtual LPBYTE GetTaskUploadRes()=0;

	//������ȡ�ͻ�����ϢӦ������
//	virtual LPBYTE GetTaskStartRes()=0;


///////��������ڵ����Ӧ������

	//�������ܳɹ�Ӧ������
//	virtual LPBYTE GetTaskUploadRes()=0;

	//�����������δ�ҵ�����Ӧ������
//	virtual LPBYTE GetTaskStartRes()=0;

	//���������½�������Ӧ������
//	virtual LPBYTE GetTaskUploadRes()=0;

	//��������ʧ��Ӧ������
//	virtual LPBYTE GetTaskStartRes()=0;

	//��ȡ�������ڵ���Ϣ
//	CompNodeInfo *ReqGetClientList();

	//�������ڵ�����

	//������ܳɹ��������
//	WorkItemGuid * ReqDecRecovered(CompWIRecovered *pwirec);

	//���������ɣ���λ�ҵ����
//	WorkItemGuid * ReqDecUnRecovered(CompWIUnRecovered *pwiunrec);

	//������������
//	WorkItemInfo * ReqWorkitems(CompGuid *pguid);
	

	//�������ʧ��

	
/*	
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
*/

	
};
