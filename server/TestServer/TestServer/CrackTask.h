#pragma once

#include <afxmt.h>
#include <map>
#include <vector>
#include "algorithm_types.h"
#include "CrackBlock.h"
#include "CrackHash.h"

#define CRACK_TASK_ERR -2000
#define NOT_SUPPORT_CT_STATUS CRACK_TASK_ERR+ 100

#define NOT_CONVERT_TO_RUNNING CRACK_TASK_ERR+ 101

#define NOT_CONVERT_TO_READY CRACK_TASK_ERR+ 102

#define NOT_CONVERT_TO_DELETE CRACK_TASK_ERR+ 103
#define NOT_CONVERT_TO_PAUSED CRACK_TASK_ERR+ 104

#define NOT_CONVERT_TO_FINISHED CRACK_TASK_ERR+ 105

#define NOT_CONVERT_TO_FAIL CRACK_TASK_ERR+106

#define CH_LEN sizeof(crack_hash)

#define HASH_NUM_IN_TASK 100


//HASH运行状态


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

	//出始化相关内容
	int Init(crack_task *pCrackTask);

	int SplitTaskFile(char *pguid);
	
	int SetStatus(char status);

	CCrackBlock *GetAReadyWorkItem();
	
	int updateStatusToFinish(struct crack_result *result,int hash_index);

	void calcProgressByBlock();

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

	//带解密工作项列表
//	std::vector <CCrackBlock *> m_crackblock_list;

	CB_MAP m_crackblock_map;

	CCriticalSection m_crackblock_cs;



	CRACK_HASH_LIST m_crackhash_list;


	//任务的动态状态信息
	unsigned char m_status;

	//切分数目
	unsigned int m_split_num;
	
	//完成数据
	unsigned int m_finish_num;

	//解密成功标志
	bool m_bsuccess;

	//解密进度0.0 ~ 100.0
	float	m_progress;

	//解密成功后，保存解密后结果
	unsigned char m_result[32];

	//上传该任务的控制节点guid
	unsigned char m_control_guid[40];

	//任务优先级
	unsigned char m_priority;

	//任务workitem 是否都已经启动过
	unsigned int m_runing_num;



	//文件句柄
	void *m_file;


	//文件长度
	unsigned int m_filelen;

	//......
	
};
