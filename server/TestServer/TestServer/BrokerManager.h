#pragma once

#include "Common.h"
#include "guidgenerator.h"
#include "ClientRequest.h"
#include "Broker.h"
#include "TaskManager.h"
#include "WorkItemManager.h"
#include "SimpleTaskSchedule.h"

#include "CLog.h"


class CBrokerManager : public CTaskManager
{

public:
	CBroker serverBroker;

public:
	CBrokerManager(void);
	~CBrokerManager(void);

	CTask * GetTaskByGuid(GUID guid);
	CTask * GetTaskByGuid(std::string guid);
	CTask * GetTaskByStatus(BYTE status);
	
	
	//������ƽڵ�����
	std::string ReqUploadTask(CtlTaskUpload *ptaskupload);  //���������ϴ�����
	
	//��������ʼ����
	INT	ReqStartTask(CtlTaskStart *ptaskstart);
	
	//��������ֹͣ����
	INT ReqStopTask(CtlTaskGuid *ptaskguid);
	
	//��������ɾ������
	INT ReqDelTask(CtlTaskGuid *ptaskguid);
	

	//����������ͣ����
	INT ReqPauseTask(CtlTaskGuid *ptaskguid);

	//��ȡ����ִ�н��
	TaskResult * ReqGetTaskResult(CtlTaskGuid *ptaskguid);

	//��ȡ���ڽ�������״̬
	TaskStatus *ReqGetTaskStatus();

	//��ȡ�������ڵ���Ϣ
	CompNodeInfo *ReqGetClientList();

	//�������ڵ�����

	//������ܳɹ��������
	WorkItemGuid * ReqDecRecovered(CompWIRecovered *pwirec);

	//���������ɣ���λ�ҵ����
	WorkItemGuid * ReqDecUnRecovered(CompWIUnRecovered *pwiunrec);

	//������������
	WorkItemInfo * ReqWorkitems(CompGuid *pguid);
	

	//�������ʧ��
	INT ReqDecFailure(CompWIFailed *pwifailed);
		
	//CTask * CreateWorkItem();	//����������



	//�����з��㷨
	int splitByCondition();


	//��������㷨,��һ�������б���ѡ���ִ�е�����
	CTask * taskSchedule(std::vector<CTask *> tlist);

	//��̬���ٿռ�ӿ�
	LPVOID _Alloc(UINT size);
	VOID _Free(LPVOID pt);

};
