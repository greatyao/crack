#pragma once

#include <afxmt.h>
#include <map>
#include <vector>
#include "algorithm_types.h"
#include "CrackBlock.h"


#define CRACK_TASK_ERR -2000
#define NOT_SUPPORT_CT_STATUS CRACK_TASK_ERR+ 100

#define NOT_CONVERT_TO_RUNNING CRACK_TASK_ERR+ 101

#define NOT_CONVERT_TO_READY CRACK_TASK_ERR+ 102

#define NOT_CONVERT_TO_DELETE CRACK_TASK_ERR+ 103
#define NOT_CONVERT_TO_PAUSED CRACK_TASK_ERR+ 104

#define NOT_CONVERT_TO_FINISHED CRACK_TASK_ERR+ 105

#define CH_LEN sizeof(crack_hash)

#define HASH_NUM_IN_TASK 100

struct CBMapLessCompare{
	
	bool operator()(const char * str1,const char *str2) const
	{

		return strcmp(str1,str2) < 0;
	}


};

typedef std::map<char *, CCrackBlock *,CBMapLessCompare> CB_MAP;


class CCrackTask : public crack_task
{
public:
	CCrackTask(void);
	~CCrackTask(void);

	//��ʼ���������
	int Init(crack_task *pCrackTask);

	int SplitTaskFile(char *pguid);
	
	int SetStatus(char status);


	CCrackBlock *GetAReadyWorkItem();
	
	int updateStatusToFinish(struct crack_result *result);

	void * Alloc(int size);
	void Free(void *p);

private:

	void resetProgress();
	int updateStatusToRunning();
	int updateStatusToStop();
	int updateStatusToDel();
	int updateStatusToPause();
	
	int updateStatusToFail();
int setCrackBlockStatus(char status);
	


public:

	//�����ܹ������б�
//	std::vector <CCrackBlock *> m_crackblock_list;

	CB_MAP m_crackblock_map;

	CCriticalSection m_crackblock_cs;



	//����Ķ�̬״̬��Ϣ
	unsigned char m_status;

	//�з���Ŀ
	unsigned int m_split_num;
	
	//�������
	unsigned int m_finish_num;

	//���ܳɹ���־
	bool m_bsuccess;

	//���ܽ���0.0 ~ 100.0
	float	m_progress;

	//���ܳɹ��󣬱�����ܺ���
	unsigned char m_result[32];

	//�ϴ�������Ŀ��ƽڵ�guid
	unsigned char m_control_guid[40];

	//�������ȼ�
	unsigned char m_priority;

	//����workitem �Ƿ��Ѿ�������
	unsigned int m_runing_num;

	//......
	
};
