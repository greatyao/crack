#pragma once

#include <vector>
#include <map>
#include <deque>
#include <string>

#include "CrackTask.h"
#include "CLog.h"
#include "ReqPacket.h"
#include "ResPacket.h"
#include "macros.h"
#include "BlockNotice.h"

using std::string;
class CClientInfo;

struct MapLessCompare{
	bool operator()(const char * str1,const char *str2) const
	{
		return strcmp(str1,str2) < 0;
	}

	bool operator()(const string& str1,const string& str2) const
	{
		return strcmp(str1.c_str(),str2.c_str()) < 0;
	}
};

//typedef std::map<char *, CCrackBlock *,MapLessCompare > TOTAL_CB_MAP;

typedef std::vector<char *> GUID_VECTOR;

typedef std::vector<CBlockNotice *> CBN_VECTOR;

typedef std::vector<CClientInfo *> CI_VECTOR;

typedef std::map<char *,CCrackTask *,MapLessCompare> CT_MAP;

typedef std::map<string,CBN_VECTOR,MapLessCompare> CCB_MAP; //computer<----> block ӳ��

//typedef std::queue<char *> CT_QUEUE;
typedef std::deque<char *> CT_DEQUE;

class CClientInfo;

class CCrackBroker
{
public:
	CCrackBroker(void);
	~CCrackBroker(void);

	//��½
	int ClientLogin2(const void* data, const char* ip, int port, unsigned int sock, CClientInfo ** res);

	//keepalive
	int ClientKeepLive2(const char *ip, void* s, unsigned char* cmd, void** data);

	//�˳�
	int DoClientQuit(const char *ip,int port);

	/////////////////////////////////////////////////////////////////////////
	//////////////////////	���ƽڵ�ҵ���߼������� /////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//��δ����������з�
	int	CreateTask(struct crack_task *pReq, void* pclient);

	//�ļ��ϴ��ɹ��󣬶���������з�
	int SplitTask(const char *guid, const char* john = NULL);

	int	StartTask(struct task_start_req *pReq, void* pclient);
	
	int StopTask(struct task_stop_req *pReq, void* pclient);
	int DeleteTask(struct task_delete_req *pReq, void* pclient);

	int PauseTask(struct task_pause_req *pReq, void* pclient);
	int GetTaskResult(struct task_result_req *pReq,struct task_result_info **pRes,int *resNum, void* pclient);

	int GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum, void* pclient);
	int GetClientList(struct compute_node_info **pRes,unsigned int *resNum);

	int deleteTask(const char *guid, void* pclient);	//�����з�����ʧ�ܣ�ɾ���մ���������

	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////	����ڵ�ҵ���߼������� /////////////////////////
	/////////////////////////////////////////////////////////////////////////
	
	int GetAWorkItem2(const char *ipinfo, struct crack_block **pRes); //���Ӵ����������ڵ�ip��ַ�Ͷ˿�

	int GetWIStatus(struct crack_status *pReq);

	int GetWIResult(struct crack_result *pReq);

	int QueryTaskByWI(char* task_guid, const char* block_guid);

	/////////////////////////////////////////////////////////////////////////
	//////////////////////		�ڴ洦����		/////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void *Alloc(int size);
	void Free(void *p);

	static void GetTaskFileByGuid(const char* guid, char* fn, int n);

private:
	int GetComputeNodesNum();
	int removeFromQueue(const char *guid);

	int getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes);	//���ص������
	int	getResultFromTaskNew(CCrackTask *pCT,struct task_result_info *pRes); //���ض�����

	
	int getStatusFromTask(CCrackTask *pCT,struct task_status_info *pRes);

	int getBlockFromCrackBlock(CCrackBlock *pCB,struct crack_block *pRes);

	void updateReadyQueue(CCrackBlock *pCB);

	void checkReadyQueue(CCrackTask *pCT);


	///add the block<---->computer op
	//int addNewCompBlock(char *ipinfo,char *blockguid,char status);
	int deleteCompBlock(const char *ipinfo,char *blockguid);
	int setCompBlockStatus(const char *ipinfo,char *blockguid,char status);

	int getBlockByComp(const char *ipinfo,CBN_VECTOR& cbnvector,char status);

	int setNoticByHash(CCrackBlock *pCB,int index);

public :
	
	//CCriticalSection m_cracktask_cs;
	
	//������ٲ�������
	CT_MAP m_cracktask_map;

	//ѭ�����У������������
	CT_DEQUE m_cracktask_ready_queue;

	//CCriticalSection m_client_cs;
	CI_VECTOR m_client_list;

	//CCriticalSection m_total_crackblock_cs;

	CB_MAP m_total_crackblock_map;

	//����ڵ��block ��ӳ��
	CCB_MAP m_comp_block_map;

};

extern CCrackBroker g_CrackBroker;

