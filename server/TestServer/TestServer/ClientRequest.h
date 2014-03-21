
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



#endif 
