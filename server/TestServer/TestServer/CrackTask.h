#pragma once

#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include "algorithm_types.h"
#include "CrackBlock.h"
#include "CrackHash.h"

#define CH_LEN sizeof(crack_hash)

#define HASH_NUM_IN_TASK 100


//HASH����״̬


struct CBMapLessCompare{
	
	bool operator()(const char * str1,const char *str2) const
	{

		return strcmp(str1,str2) < 0;
	}
};

typedef std::map<char *, CCrackBlock *,CBMapLessCompare> CB_MAP;
typedef std::vector<CCrackHash *> CRACK_HASH_LIST;


class CCrackTask : public crack_task
{
public:
	CCrackTask(void);
	~CCrackTask(void);

	//��ʼ���������
	int Init(crack_task *pCrackTask);

	int SplitTaskFile(const char *guid, const char* john = NULL);
	
	int SetStatus(char status);

	CCrackBlock *GetAReadyWorkItem();


	CCrackBlock *GetAReadyWorkItem2(const char *ipinfo);

	
	int updateStatusToFinish(struct crack_result *result,int hash_index);

	void calcProgressByBlock();

	void RefreshRemainTime();

	void startTime();

	void * Alloc(int size);
	void Free(void *p);

private:

	void resetProgress();
	int updateStatusToRunning();
	int updateStatusToStop();
	int updateStatusToDel();
	int updateStatusToPause();
	
	int updateStatusToFail();
	int setCrackBlockStatus(char status,int hash_index);

	int setFinishByHash();

	void setCrackBlockUncrack(int hash_index);

	int checkBlockOfHash(int hash_index);
	
public:

	//�����ܹ������б�
//	std::vector <CCrackBlock *> m_crackblock_list;

	CB_MAP m_crackblock_map;

	CB_MAP::iterator cur_crack_block;

	//CCriticalSection m_crackblock_cs;
	char m_owner[64]; //task�Ĵ�����


	CRACK_HASH_LIST m_crackhash_list;


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

	//�����ٶ�
	float	m_speed;

	//���ܳɹ��󣬱�����ܺ���
	unsigned char m_result[32];

	//�ϴ�������Ŀ��ƽڵ�guid
	unsigned char m_control_guid[40];

	//�������ȼ�
	unsigned char m_priority;

	//����workitem �Ƿ��Ѿ�������
	unsigned int m_runing_num;


	unsigned int m_start_time;  //���һ�ο�ʼʱ��

	unsigned int m_running_time;  //�Ѿ�����ʱ��

	unsigned int m_remain_time;   //��ʣʱ��

	//�ļ����
	void *m_file;


	//�ļ�����
	unsigned int m_filelen;

	//......
	
};
