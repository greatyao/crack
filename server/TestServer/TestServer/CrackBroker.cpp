#include "CrackBroker.h"
#include "CompClient.h"
#include "ClientInfo.h"
#include "err.h"
#include "CLog.h"
#include "PersistencManager.h"
#include "CrackTask.h"
#include "CrackHash.h"
#include "CrackBlock.h"
#include "CrackBroker.h"
#include "BlockNotice.h"
#include <Shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")

CCrackBroker::CCrackBroker(void)
{
}

CCrackBroker::~CCrackBroker(void)
{
}

//�ӳ־û������������
int CCrackBroker::LoadFromPersistence(bool use_leveldb)
{
	char path[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, path, MAX_PATH);  
	char *p = strrchr(path, '\\');  
	*p=0x00;
	strncat(path, "\\taskdb", sizeof(path));

	g_Persistence.OpenDB(path, use_leveldb);
	g_Persistence.LoadTaskMap(m_cracktask_map);
	g_Persistence.LoadHash(m_cracktask_map);
	g_Persistence.LoadBlockMap(m_total_crackblock_map, m_cracktask_map);
	g_Persistence.LoadReadyTaskQueue(m_cracktask_ready_queue, m_cracktask_map);
	g_Persistence.LoadNoticeMap(m_comp_block_map);

	for(CT_MAP::iterator it = m_cracktask_map.begin(); it !=m_cracktask_map.end(); it++)
	{
		CCrackTask* pCT = it->second;
		pCT->InitAvailableBlock();
		pCT->calcProgressByBlock();
		pCT->RefreshRemainTime();
	}

	return 0;
}

//�����¼
int CCrackBroker::ClientLogin2(const void* data, const char* ip, int port, unsigned int sock, CClientInfo ** res)
{
	CClientInfo *pCI = NULL;
	client_login_req login;
	memcpy(&login, data, sizeof(login));
	
	//TODO:������Ҫ��֤client

	if(login.m_type == COMPUTE_TYPE_CLIENT)
		pCI = new CCompClient;
	else
	{
		pCI = new CClientInfo;
		if(strcmp(login.m_user, "trimpsadmin") == 0 && strcmp(login.m_password, "trimps1234qwer!@#$") == 0)
			login.m_type = SUPER_CONTROL_TYPE_CLIENT;
	}
	pCI->Init(&login, ip, port, sock);
	m_client_list.push_back(pCI);
	*res = pCI;

	//���client֮ǰ�˳����ٴε�½,����Ҫ������ǰ������task���½ӹ�
	for(CT_MAP::iterator it = m_cracktask_map.begin(); it!= m_cracktask_map.end(); it++)
	{
		if(strcmp(it->second->m_owner, pCI->GetOwner()) == 0)
			pCI->InsetTask(it->first, it->second);
	}
	
	return 0;
}

//��������
int CCrackBroker::ClientKeepLive2(const char *owner, void* s, unsigned char* cmd, void** data)
{
	SOCKET sock = (SOCKET)s;
	CClientInfo *pCI = NULL;
	CBN_VECTOR tmpcbn;
	int len = 0;
	keeplive_compclient* ka = NULL;

	for (int i = 0 ;i < m_client_list.size(); i ++){
		if(m_client_list[i]->m_clientsock == sock)
		{
			pCI= m_client_list[i];
			break;
		}
	}

	if(pCI == NULL)
	{
		for (int i = 0 ;i < m_client_list.size(); i ++){
			CClientInfo* client = m_client_list[i];
			if(strcmp(owner, client->GetOwner()) == 0)
			{
				pCI = client;
				break;
			}
		}
	}

	//���Ǽ���ڵ㣬ֱ�ӷ��ͼ򵥵�������Ӧ��
	if(pCI == NULL || pCI->m_type != COMPUTE_TYPE_CLIENT)
	{
		*cmd = CMD_REPLAY_HEARTBEAT;
		return 0;
	}


	//�����Ҫ֪ͨ��block�б�
	getBlockByComp(pCI->GetOwner(),tmpcbn,STATUS_NOTICE_FINISH | STATUS_NOTICE_STOP);
	int size = tmpcbn.size();


	///////////////////////////////

	//int noneed = 0;//������Ҫ���н��ܵ�workitem��Ŀ����Ҫ����ÿ������ڵ��block/task֮��Ĺ�ϵ����ó�
	len = sizeof(keeplive_compclient)+size*sizeof(keeplive_compclient::block_guid);
	ka = (keeplive_compclient*)Alloc(len);
	ka->tasks = m_cracktask_ready_queue.size();
	ka->blocks = size;
	for(int i = 0; i < size; i++)
	{
		//��ka->guids���и�ֵ
		memset(ka->guids[i],0,40);
		memcpy(ka->guids[i],tmpcbn[i]->m_guid,sizeof(tmpcbn[i]->m_guid));
	
		//����BlockNotice��Դ
		delete tmpcbn[i];
	}



	*cmd = CMD_COMP_HEARTBEAT;
	*data = ka;
	return len;
}

//���ƽڵ�ҵ���߼�������

//����������
int	CCrackBroker::CreateTask(struct crack_task *pReq, void* pclient){
	CClientInfo* client = (CClientInfo*)pclient;
	int ret = 0;
	CCrackTask *pTask = NULL;
	CT_MAP::iterator temp_iter;

	pTask = new CCrackTask;
	if (!pTask){

		CLog::Log(LOG_LEVEL_WARNING, "Allocate object CCrackTask Error\n");
		return ERR_OUTOFMEMORY;

	}
	ret = pTask->Init(pReq);
	
//	m_cracktask_cs.Lock();
	
	m_cracktask_map.insert(CT_MAP::value_type(pTask->guid,pTask));
	client->InsetTask(pTask->guid, pTask);
	strncpy(pTask->m_owner, client->GetOwner(), sizeof(pTask->m_owner));

//	m_cracktask_cs.Unlock();
	return ret;
}

//�з�����ӿ�
int CCrackBroker::SplitTask(const char *guid, const char* john){
	
	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
		
	iter_task = m_cracktask_map.find((char*)guid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"SplitTask: Can't find task with guid %s\n",guid);
		return ERR_NO_THISTASK;
	}

	pCT = iter_task->second;
	
	ret = pCT->SplitTaskFile(guid, john);
	if (ret < 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"SplitTask: Split task file %s Error %d\n",guid, ret);
		return ret;
		
	}

	m_total_crackblock_map.insert(pCT->m_crackblock_map.begin(),pCT->m_crackblock_map.end());

	//OK,���ڽ���־û�
	g_Persistence.PersistTask(pCT, CPersistencManager::Insert);
	g_Persistence.PersistHash(pCT->guid, pCT->m_crackhash_list, CPersistencManager::Insert);
	g_Persistence.PersistBlockMap(pCT->m_crackblock_map, CPersistencManager::Insert);

	return ret;
}

//��ʼ������
int	CCrackBroker::StartTask(struct task_start_req *pReq, void* pclient){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
	CClientInfo* client = (CClientInfo*)pclient;

	if (!pReq){

		CLog::Log(LOG_LEVEL_DEBUG,"StartTask: Req is NULL Error\n");
		return ERR_INVALID_PARAM;

	}

	if(!client->OwnTask((char *)pReq->guid)){
		CLog::Log(LOG_LEVEL_WARNING, "StartTask: User %s has no priviledge to control\n", client->GetOwner());
		return ERR_PRIVILEDGE;
	}
	
//	m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"StartTask: Can't find task with guid %s\n",pReq->guid);
		return ERR_NO_THISTASK;
	}else{
		
		pCT = iter_task->second;
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_RUNNING);

		//��״̬�־û�
		g_Persistence.PersistHash(pCT->guid, pCT->m_crackhash_list, CPersistencManager::Update);
		g_Persistence.PersistBlockMap(pCT->m_crackblock_map, CPersistencManager::Update);
		g_Persistence.PersistTask(pCT, CPersistencManager::Update);
		
		//���񱻷���ѭ�����ж�β���ȴ�����
		if(ret == 0){	//����ȷ��SetStatusִ�гɹ�
			m_cracktask_ready_queue.push_back(pCT->guid);
			CLog::Log(LOG_LEVEL_NOMAL,"StartTask: Set %s OK\n", pCT->guid);

			g_Persistence.PersistReadyTaskQueue(m_cracktask_ready_queue);
		}
	}
//	m_cracktask_cs.Unlock();
	
	return ret;
}
	
int CCrackBroker::StopTask(struct task_stop_req *pReq, void* pclient){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end;
	CCrackBlock *pCB = NULL;
	CCrackTask *pCT = NULL;
	CClientInfo* client = (CClientInfo*)pclient;

	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"StopTask: Req NULL Error\n");
		return ERR_INVALID_PARAM;

	}
	if(!client->OwnTask((char *)pReq->guid)){
		CLog::Log(LOG_LEVEL_WARNING, "StopTask: User %s has no priviledge to control\n", client->GetOwner());
		return ERR_PRIVILEDGE;
	}
	
	//m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"StopTask: Can't find task with guid %s\n",pReq->guid);
		return ERR_NO_THISTASK;
	}else{
		
		pCT = iter_task->second;
		
		//���������е�block ���������ӳ����
		iter_block_end = pCT->m_crackblock_map.end();
		for(iter_block = pCT->m_crackblock_map.begin();iter_block!= iter_block_end;iter_block++){
			
			pCB = iter_block->second;
			if ((pCB->m_status == WI_STATUS_RUNNING) || (pCB->m_status == WI_STATUS_LOCK)){
				
				setCompBlockStatus(pCB->m_comp_guid,pCB->guid,STATUS_NOTICE_STOP);

			}

		}
		//////////////////////////////
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_READY);

		//��״̬�־û�
		g_Persistence.PersistHash(pCT->guid, pCT->m_crackhash_list, CPersistencManager::Update);
		g_Persistence.PersistBlockMap(pCT->m_crackblock_map, CPersistencManager::Update);
		g_Persistence.PersistTask(pCT, CPersistencManager::Update);
		
	}
	
	//���ֹͣ����ɹ�
	if (ret == 0){

		removeFromQueue((char*)pReq->guid);
	}
	
	
	//m_cracktask_cs.Unlock();
	return ret;
}

int CCrackBroker::DeleteTask(struct task_delete_req *pReq, void* pclient){
	
	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end;
	CCrackBlock *pCB = NULL;
	CClientInfo* client = (CClientInfo*)pclient;
	
	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"DeleteTask: Req NULL Error\n");
		return ERR_INVALID_PARAM;
	}

	if(!client->OwnTask((char *)pReq->guid)){
		CLog::Log(LOG_LEVEL_WARNING, "DeleteTask: User %s has no priviledge to control\n", client->GetOwner());
		return ERR_PRIVILEDGE;
	}
	
//	m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"DeleteTask: Can't find task with guid %s\n",pReq->guid);
		return ERR_NO_THISTASK;

	}else{
		
		pCT = iter_task->second;
		
		//��������������еĻ���������ɾ��
		if (pCT->m_status == CT_STATUS_RUNNING){

			CLog::Log(LOG_LEVEL_WARNING,"DeleteTask: Task %s is Running, Can't delete!\n",pReq->guid);
			return ERR_CONVERTDELETE;

		}
		//Task status --> Running , the block status --> ready
		//���ص�ɾ����������wi ״̬�������
	//	ret = pCT->SetStatus(CT_STATUS_DELETED);


		//֪ͨ�������е�blockֹͣ����
		//���������е�block ���������ӳ����
		iter_block_end = pCT->m_crackblock_map.end();
		for(iter_block = pCT->m_crackblock_map.begin();iter_block!= iter_block_end;iter_block++){
			
			pCB = iter_block->second;
			if ((pCB->m_status == WI_STATUS_RUNNING) || (pCB->m_status == WI_STATUS_LOCK)){
				
				setCompBlockStatus(pCB->m_comp_guid,pCB->guid,STATUS_NOTICE_STOP);

			}

		}
		//////////////////////////////
		//��״̬�־û���������deleteTask֮ǰ���ã�������
		g_Persistence.PersistHash(pCT->guid, pCT->m_crackhash_list, CPersistencManager::Delete);
		g_Persistence.PersistBlockMap(pCT->m_crackblock_map, CPersistencManager::Delete);
		g_Persistence.PersistTask(pCT, CPersistencManager::Delete);

		//����ɾ��,ֱ��ɾ��������񣬼���hash ��crackblock
		this->deleteTask((char *)pReq->guid, pclient);
	}

	//�ӵ��ȶ������Ƴ�
	removeFromQueue((char*)pReq->guid);
	
	//m_cracktask_cs.Unlock();
	return ret;
}

int CCrackBroker::PauseTask(struct task_pause_req *pReq, void* pclient){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
	CClientInfo* client = (CClientInfo*)pclient;

	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"PauseTask: Req NULL Error\n");
		return ERR_INVALID_PARAM;
	}
	if(!client->OwnTask((char *)pReq->guid)){
		CLog::Log(LOG_LEVEL_WARNING, "PauseTask: User %s has no priviledge to control\n", client->GetOwner());
		return ERR_PRIVILEDGE;
	}
	
//	m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"PauseTask: Can't find task with guid %s\n",pReq->guid);
		return ERR_NO_THISTASK;

	}else{
		
		pCT = iter_task->second;
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_PAUSED);

		//��״̬�־û�
		g_Persistence.PersistHash(pCT->guid, pCT->m_crackhash_list, CPersistencManager::Update);
		g_Persistence.PersistBlockMap(pCT->m_crackblock_map, CPersistencManager::Update);
		g_Persistence.PersistTask(pCT, CPersistencManager::Update);

	}
	
	//�ӵ��ȶ������Ƴ�
	 removeFromQueue((char*)pReq->guid);
	
//	m_cracktask_cs.Unlock();
	return ret;
}

int CCrackBroker::GetTaskResult(struct task_result_req *pReq,struct task_result_info **pRes,int *resNum, void* pclient){

	int ret = 0;
	CT_MAP::iterator iter_task;
	int hashnum = 0;
	CCrackTask *pCT = NULL;
	struct task_result_info *pres = NULL;
	CClientInfo* client = (CClientInfo*)pclient;
	char* owner = client->GetOwner();
	bool super = client->SuperUser();
	
	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"GetTaskResult: Can't find task with guid %s\n",pReq->guid);
		return ERR_NO_THISTASK;
	}else{

		pCT = iter_task->second;
		if(!super && strcmp(pCT->m_owner, owner) != 0){
			return ERR_PRIVILEDGE;
		}

		hashnum = pCT->m_crackhash_list.size();

		pres = (struct task_result_info *)Alloc(sizeof(struct task_result_info)*hashnum);
		if (!pres){
			
			CLog::Log(LOG_LEVEL_WARNING,"GetTaskResult: Alloc task_result_info object Error\n");
			return ERR_OUTOFMEMORY;
		}

		//�õ�������
	//	getResultFromTask(pCT,pres);

		ret = getResultFromTaskNew(pCT,pres);

		*pRes = pres;
		*resNum = ret;
		ret = 0;
	}

//	m_cracktask_cs.Unlock();

	return ret;
}

int CCrackBroker::GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum, void* pclient){
	
	int ret = 0;
	int task_num = 0;
	CT_MAP::iterator iter_task;
	struct task_status_info *pres = NULL;
	CCrackTask *pCT = NULL;
	int j = 0;
	CClientInfo* client = (CClientInfo*)pclient;
	char* owner = client->GetOwner();
	bool super = client->SuperUser();

	task_num = m_cracktask_map.size();

	pres = (struct task_status_info *)Alloc(sizeof(struct task_status_info)*task_num);
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"GetTasksStatus: Alloc task_status_info object Error\n");
		return ERR_OUTOFMEMORY;
	}
	
	for(iter_task = m_cracktask_map.begin();iter_task != m_cracktask_map.end();iter_task++){
		
		pCT = iter_task->second;
		//�����û����߸�task�Ĵ�����
		if(super || strcmp(owner, pCT->m_owner) == 0)
		{
			getStatusFromTask(pCT,&pres[j]);
			j++;
		}
	}

	*pRes = pres;
	*resNum = j;

	return ret;
}

int CCrackBroker::GetClientList(struct compute_node_info **pRes,unsigned int *resNum){

	int ret = 0;
	int client_num = 0;
	int size = 0;
	CI_VECTOR::iterator iter_client;
	CClientInfo *pCI = NULL;
	struct compute_node_info *pres = NULL;
	int j =  0;
	int i = 0;

//	m_client_cs.Lock();

	size = m_client_list.size();

	client_num = GetComputeNodesNum();
	
	pres = (struct compute_node_info *)Alloc(sizeof(struct compute_node_info)*client_num);
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_DEBUG,"GetClientList: Alloc compute_node_info Error\n");
	//	m_client_cs.Unlock();
		return ERR_OUTOFMEMORY;
	}
	
	memset(pres,0,sizeof(struct compute_node_info) * client_num);

	for(i=0 ;i < size ;i++){

		pCI = m_client_list[i];

		if (pCI->m_type != COMPUTE_TYPE_CLIENT)
			continue;

		//��CClientInfo �õ� computer_node_info 
		
		memcpy(pres[j].guid,pCI->m_guid,40);
		memcpy(pres[j].hostname,pCI->m_hostname,sizeof(pres[j].hostname));
		memcpy(pres[j].ip,pCI->m_ip,20);
		memcpy(pres[j].os,pCI->m_osinfo,sizeof(pres[j].os));

		pres[j].gputhreads = ((CCompClient *)pCI)->m_gputhreads;
		pres[j].cputhreads = ((CCompClient *)pCI)->m_cputhreads;

		CLog::Log(LOG_LEVEL_DEBUG, "CompClient %d: OS=\"%s\" Host=\"%s\" [%d %d]\n", j, pres[j].os, pres[j].hostname,
			pres[j].gputhreads, pres[j].cputhreads );
	
		j++;
	}
	
	*pRes = pres;
	*resNum = j;

//	m_client_cs.Unlock();
	return ret;
}

//����ڵ�ҵ���߼�������

//�����ӵĻ�ȡblock �ĺ���������˶Լ���ڵ��block ֮��Ķ�Ӧ��ϵ
int CCrackBroker::GetAWorkItem2(const char *worker,struct crack_block **pRes){

	int ret = 0;
	CT_MAP::iterator iter_task;
	struct crack_block *pres = NULL;
	CCrackBlock *pTmpCB = NULL;
	CCrackTask *pCT = NULL;
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;
	CCB_MAP::iterator comp_iter;
	CBN_VECTOR tmpcbn;
	CBlockNotice *pBN = NULL;


	string guid;
	int size = 0;

	pres = (struct crack_block *)Alloc(sizeof(struct crack_block));
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"GetAWorkItem2: Alloc crack_block object error\n");
		return ERR_OUTOFMEMORY;
	}
	
//	m_cracktask_cs.Lock();
	
	memset(pres,0,sizeof(struct crack_block));
	
	size = m_cracktask_ready_queue.size();
	if (size < 1){	
	//	m_cracktask_cs.Unlock();
		return ERR_NOREADYITEM;
	}

	guid = m_cracktask_ready_queue.front();
	CLog::Log(LOG_LEVEL_WARNING,"Front task guid = %s\n", guid.c_str());
	if (guid.empty()){

		CLog::Log(LOG_LEVEL_WARNING,"GetAWorkItem2: Available task is empty\n");
//		m_cracktask_cs.Unlock();
		return ERR_NOREADYITEM;
	}

	iter_task = m_cracktask_map.find((char*)guid.c_str());
	if (iter_task == m_cracktask_map.end()){

		CLog::Log(LOG_LEVEL_WARNING,"GetAWorkItem2: Can't find task with guid %s\n", guid.c_str());
		m_cracktask_ready_queue.pop_front();
		ret =  ERR_NOREADYITEM;
//		m_cracktask_cs.Unlock();

		//�������г־û�
		g_Persistence.PersistReadyTaskQueue(m_cracktask_ready_queue);
		
		return ret;
	}

	pCT = iter_task->second;
	
	pCB = pCT->GetAReadyWorkItem2(worker);
	if (pCB == NULL){
		
		CLog::Log(LOG_LEVEL_WARNING,"GetAWorkItem2: Can't find ready item from task %s\n", guid.c_str());
		m_cracktask_ready_queue.pop_front();
		return ERR_NOREADYITEM;
	}
	
	if (pCT->m_split_num == pCT->m_runing_num){

		m_cracktask_ready_queue.pop_front();
		//�������г־û�
		g_Persistence.PersistReadyTaskQueue(m_cracktask_ready_queue);

	}else{
	
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(guid);
	}

	getBlockFromCrackBlock(pCB,pres);

	*pRes = pres;

	///����block ��comp ��Ӧ��ϵ
	//STATUS_NOTICE_RUN
	strncpy(pCB->m_comp_guid,  worker, sizeof(pCB->m_comp_guid));
	g_Persistence.UpdateOneBlock(pCB);
	ret = setCompBlockStatus(worker,pCB->guid,STATUS_NOTICE_RUN);
	CLog::Log(LOG_LEVEL_NOMAL,"GetAWorkItem2: Add Map <%s, %s> %s\n",worker,pCB->guid,(ret<0?"Error":"OK"));
	
//	m_cracktask_cs.Unlock();

	return ret;
}

int CCrackBroker::QueryTaskByWI(char* task_guid, const char* block_guid)
{
	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;
	CCrackTask *pCT = NULL;

	iter_block = m_total_crackblock_map.find((char*)block_guid);
	if (iter_block == m_total_crackblock_map.end()){

		CLog::Log(LOG_LEVEL_DEBUG,"QueryTaskByWI: Can't find item with guid %s\n", block_guid);
		return  ERR_NO_THISITEM;
	}

	pCB = iter_block->second;
	pCT =(CCrackTask *)pCB->task;
	if(!pCT) return ERR_NO_THISTASK;
	
	strcpy(task_guid, pCT->guid);
	return 0;
}

int CCrackBroker::GetWIStatus(struct crack_status *pReq){

	int ret = 0;
	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;
	CCrackTask *pCT = NULL;


	iter_block = m_total_crackblock_map.find(pReq->guid);
	if (iter_block == m_total_crackblock_map.end()){

		CLog::Log(LOG_LEVEL_DEBUG,"GetWIStatus: Can't find item with guid %s %d\n",pReq->guid, pReq->progress);
		return ERR_NO_THISITEM;
	}

		
	pCB = iter_block->second;

	pCB->m_progress = pReq->progress;
	pCB->m_speed = pReq->speed;
	pCB->m_remaintime = pReq->remainTime;

	pCT = (CCrackTask *)(pCB->task);
	
	pCT->RefreshRemainTime();

	return ret;
}

int CCrackBroker::GetWIResult(struct crack_result *pReq){

	int ret = 0;
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;
	CCrackTask *pCT = NULL;

	iter_block = m_total_crackblock_map.find(pReq->guid);
	if (iter_block == m_total_crackblock_map.end()){

		CLog::Log(LOG_LEVEL_DEBUG,"GetWIResult: Can't find item with guid %s\n",pReq->guid);
		return ERR_NO_THISITEM;
	}
	
	pCB = iter_block->second;
	int index = pCB->hash_idx;
	switch(pReq->status){

		case WI_STATUS_UNLOCK:
			CLog::Log(LOG_LEVEL_NOTICE, "** Reuse item [guid=%s, algo=%d, type=%d] **\n",pReq->guid, pCB->algo, pCB->type);
			
			pCB->m_status = WI_STATUS_READY;
			((CCrackTask *)(pCB->task))->m_runing_num -=1;

			//������������и���
			updateReadyQueue(pCB);

			//blockռ���ͷţ���block--computer ӳ����ɾ��
			deleteCompBlock(pCB->m_comp_guid,pCB->guid);

			//block״̬�־û�
			pCB->m_comp_guid[0] = 0;
			g_Persistence.UpdateOneBlock(pCB);

			break;
		case WI_STATUS_RUNNING:

			pCB->m_status = WI_STATUS_RUNNING;

			CLog::Log(LOG_LEVEL_SUCCEED, "** Running item [guid=%s, tool=%s, algo=%d, type=%d] **\n", pReq->guid, pReq->password, pCB->algo, pCB->type);
			
			//�ж��Ƿ��ǵ�һ����ʼ���е�Block ,����ǵĻ����������ʱ��
			pCT = (CCrackTask *)pCB->task;
			pCT->startTime();				


			checkReadyQueue((CCrackTask *)(pCB->task));

			//block״̬�־û�
			g_Persistence.UpdateOneBlock(pCB);

			break;
		case WI_STATUS_CRACKED:
			CLog::Log(LOG_LEVEL_SUCCEED, "** Recover item [guid=%s] password=\"%s\" **\n",pReq->guid,pReq->password);
			pCB->m_status = WI_STATUS_CRACKED;
			pCT =(CCrackTask *)pCB->task;

			//����ʣ��ʱ��
			pCB->m_remaintime = 0;
			pCT->RefreshRemainTime();
						
			//���ܳɹ���ͬһ��hash�µ�block������Ҫ��������
			setNoticByHash(pCB,index);
			//ɾ�����block
			deleteCompBlock(pCB->m_comp_guid,pCB->guid);


		//	m_cracktask_cs.Lock();
			ret = pCT->updateStatusToFinish(pReq,index);

			//block/hash/task״̬�־û�
			g_Persistence.UpdateOneBlock(pCB);
			g_Persistence.UpdateOneHash(pCT->m_crackhash_list[index], pCT->guid, index);
			g_Persistence.PersistTask(pCT, CPersistencManager::Update);

			if (ret == 1){

				removeFromQueue(pCT->guid);
			}

		//	m_cracktask_cs.Unlock();
			break;
		case WI_STATUS_NO_PWD:
			CLog::Log(LOG_LEVEL_SUCCEED, "** UnRecovered item [guid=%s] NON password **\n",pReq->guid);
			
			pCB->m_status = WI_STATUS_NO_PWD;
			pCT = (CCrackTask *)pCB->task;
		//	m_cracktask_cs.Lock();

			//����ʣ��ʱ��
			pCB->m_remaintime = 0;
			pCT->RefreshRemainTime();


			//ɾ�����block
			deleteCompBlock(pCB->m_comp_guid,pCB->guid);


			ret = pCT->updateStatusToFinish(pReq,index);
			
			//task/block/hash״̬�־û�
			g_Persistence.UpdateOneBlock(pCB);
			g_Persistence.UpdateOneHash(pCT->m_crackhash_list[index], pCT->guid, index);
			g_Persistence.PersistTask(pCT, CPersistencManager::Update);


			if (ret == 1){

				removeFromQueue(pReq->guid);
			}

		//	m_cracktask_cs.Unlock();
			break;
		default:
			
			pCB->m_status = WI_STATUS_MAX;
			break;
	}
	
	return 0;
}

int CCrackBroker::GetComputeNodesNum(){
	
	int ret = 0;
	int count = 0;
	int i = 0;
	int size = m_client_list.size();
	CClientInfo *pCI = NULL;

	for (i = 0 ;i < size; i++){

		pCI = m_client_list[i];
		
		if (pCI->m_type == COMPUTE_TYPE_CLIENT ){
			count += 1;
		}
		
	}

	return count;
}

int CCrackBroker::removeFromQueue(const char *guid){

	int ret = 0;
	CT_DEQUE::iterator iter_queue;

	//��ready �������Ƴ�
	for(iter_queue = m_cracktask_ready_queue.begin();iter_queue != m_cracktask_ready_queue.end(); iter_queue ++ ){
	
		if (strncmp(iter_queue->c_str(),(char *)guid,40) == 0 ){		
			break;
		}
	}

	if (iter_queue != m_cracktask_ready_queue.end()){
			m_cracktask_ready_queue.erase(iter_queue);

			//���ȶ��г־û�
			g_Persistence.PersistReadyTaskQueue(m_cracktask_ready_queue);
	}

	return ret;
}

//�ϵ����������ش���
int CCrackBroker::getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes){
	
	int ret =0;

	pRes->status = pCT->m_status;
	memcpy(pRes->guid,pCT->guid,40);
	memcpy(pRes->password,pCT->m_result,32);

	return ret;
}


//�µ����������ش���
int CCrackBroker::getResultFromTaskNew(CCrackTask *pCT,struct task_result_info *pRes){

	int i = 0;
    int ret = 0;
	CB_MAP::iterator iter_block;
	struct task_result_info *presinfo = NULL;
	CRACK_HASH_LIST tmplist = pCT->m_crackhash_list;
	CCrackHash *pCCH = NULL;
	
	for (i = 0 ;i < tmplist.size();i ++){
		
		presinfo = &pRes[i];
		pCCH = tmplist[i];

		presinfo->status = pCCH->m_status;
		memcpy(presinfo->password,pCCH->m_result,sizeof(pCCH->m_result));
		memcpy(presinfo->john,pCCH->m_john,sizeof(pCCH->m_john));
	}
	ret = i;

	return ret;  //���ؽ����Ŀ
}
	
int CCrackBroker::getStatusFromTask(CCrackTask *pCT,task_status_info *pRes){

	int ret = 0;
	time_t mytime = time(NULL);

	memcpy(pRes->guid,pCT->guid,40);
	pRes->m_fini_number = pCT->m_finish_num;
	pRes->m_split_number = pCT->m_split_num;

	pRes->m_remain_time = pCT->m_remain_time;

	pRes->m_running_time = pCT->m_running_time;

	if (pCT->m_status == CT_STATUS_RUNNING){

		if(pCT->m_runing_num == 0)
			pRes->m_running_time = 0;
		else
			pRes->m_running_time = (pCT->m_running_time + (mytime-pCT->m_start_time));
	}

	pRes->m_algo = pCT->algo;
	//get the current block progress 
	pCT->calcProgressByBlock();
	pRes->m_progress = pCT->m_progress;
	pRes->m_speed = pCT->m_speed;
	pRes->status = pCT->m_status;

	float pro = pRes->m_progress;
	if(pro == 0) pro = 0.1;
	pRes->m_remain_time = pRes->m_running_time * (100.0/pro)-pRes->m_running_time;

	return ret;
}

int CCrackBroker::getBlockFromCrackBlock(CCrackBlock *pCB,struct crack_block *pRes){
	
	int ret = 0;
	crack_block* parent = pCB;
	memcpy(pRes, parent, sizeof(crack_block));
	pRes->task = pCB->task;
	
	return ret;
}

int CCrackBroker::DoClientQuit(const char *ip,int port){
	
	int ret = 0;
	int i = 0;
	int size = 0;
	CBN_VECTOR tmpcbn;
	CI_VECTOR::iterator iter_client;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end = m_total_crackblock_map.end();
	CCrackBlock *pCB = NULL;
	int client_num = m_client_list.size();

	for(i =0;i < client_num;i ++ ){
		
		if ((strcmp(m_client_list[i]->m_ip,ip) == 0) && (m_client_list[i]->m_port == port)){
			break;
		}
	}
	
	if (i < client_num){
		iter_client = m_client_list.begin()+i;
		m_client_list.erase(iter_client);
		
	}else{

		ret = -1;
	}

	//����ڵ��˳��Ļ�����Ҫ��������ڵ����ڼ����block״̬��ʼ��
	getBlockByComp(ip,tmpcbn,STATUS_NOTICE_RUN);

	if (tmpcbn.size() == 0){
		
		return 0;
	}
	
	//���������block ����״̬Ϊready,�������·��䣬������Ϣ���
	size = tmpcbn.size();
	for(i = 0;i < size ;i ++ ){
		
		iter_block = m_total_crackblock_map.find(tmpcbn[i]->m_guid);

		//�ͷ���Դ
		delete tmpcbn[i];

		if (iter_block == iter_block_end){

			continue;

		}

		pCB = iter_block->second;
		pCB->m_status = WI_STATUS_READY;
		pCB->m_progress = 0.0;
		pCB->m_remaintime =0;
		pCB->m_speed = 0.0;
		memset(pCB->m_comp_guid,0,40);

		//�������������������
		((CCrackTask *)(pCB->task))->m_runing_num-=1;
		updateReadyQueue(pCB);
	}

	return ret;
}

void CCrackBroker::updateReadyQueue(CCrackBlock *pCB){

	CCrackTask *pCT = (CCrackTask *)(pCB->task);
	//CT_DEQUE::iterator iter_block;
	int size = m_cracktask_ready_queue.size();
	int i = 0;

	for(i =0 ;i < size ;i ++ ){
		
		if (strcmp(m_cracktask_ready_queue[i].c_str(),pCT->guid) == 0){

			break;
		}

	}

	if (i == size){

		m_cracktask_ready_queue.push_back(pCT->guid);

		//�������г־û�
		g_Persistence.PersistReadyTaskQueue(m_cracktask_ready_queue);
	}

}

int CCrackBroker::deleteTask(const char *guid, void* pclient){
	CClientInfo* client = (CClientInfo*)pclient;
	int ret = 0;
	char filename[MAX_PATH];
	CCrackTask *pCT = NULL;
	CCrackHash *pCH = NULL;
	CCrackBlock *pCB = NULL;
	CRACK_HASH_LIST::iterator iter_hash;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_total_block;
	CB_MAP::iterator total_block_end;
	CB_MAP tmp_cb_map;

	//ɾ������Ȩ�����Ϣ
	client->EraseTask(guid, NULL);
	
	//ɾ������ļ�
	CCrackBroker::GetTaskFileByGuid(guid, filename, MAX_PATH);
	DeleteFileA(filename);

	total_block_end = m_total_crackblock_map.end();

	CT_MAP::iterator iter_task = m_cracktask_map.find((char*)guid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING, "EraseTask: Can't find Task With GUID %s\n",guid);
		return ERR_NO_THISTASK;
	}

	pCT=iter_task->second;
	//ɾ��hash �е�����
	if (pCT->m_crackhash_list.size() > 0){

		iter_hash = pCT->m_crackhash_list.begin();
		
		pCH = *iter_hash;

		delete []pCH;
		pCT->m_crackhash_list.clear();
	}

	//ɾ��crackblock ����
	if (pCT->m_crackblock_map.size() > 0){
		
		tmp_cb_map = pCT->m_crackblock_map;

		for(iter_block = tmp_cb_map.begin();iter_block!=tmp_cb_map.end();iter_block++){

			pCB = iter_block->second;

			iter_total_block = m_total_crackblock_map.find(pCB->guid);
			if (iter_total_block != total_block_end){

				m_total_crackblock_map.erase(iter_total_block);
			}
		}
		
		pCB = tmp_cb_map.begin()->second;
		tmp_cb_map.clear();

		delete [] pCB;
	}

	m_cracktask_map.erase(iter_task);
	delete pCT;

	return ret;
}

void CCrackBroker::checkReadyQueue(CCrackTask *pCT){

	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator begin_iter = pCT->m_crackblock_map.begin();
	CB_MAP::iterator end_iter = pCT->m_crackblock_map.end();
	int i = 0;

	for(iter_block = begin_iter; iter_block!=end_iter;iter_block ++ ){

		pCB = iter_block->second;
		if (pCB->m_status == WI_STATUS_READY){

			break;
		}

	}

	if (iter_block == end_iter){

		for(i = 0 ;i < m_cracktask_ready_queue.size();i ++ ){
			
			if (strcmp(pCT->guid,m_cracktask_ready_queue[i].c_str()) == 0){
					
				break;
			}
		}

		if (i < m_cracktask_ready_queue.size()){

			m_cracktask_ready_queue.erase(m_cracktask_ready_queue.begin()+i);
		}
	//	m_cracktask_ready_queue.erase(pCT->guid);

	}
}

//��ӳ�����ɾ��block
int CCrackBroker::deleteCompBlock(const char *worker,char *blockguid){

	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;

	//��������������뵽����ڵ��block ӳ�����
	//CLog::Log(LOG_LEVEL_WARNING,"CompIP Map %d %s\n", m_comp_block_map.size(), ipinfo);
	//for(comp_iter = m_comp_block_map.begin(); comp_iter != m_comp_block_map.end(); comp_iter++)
	//	CLog::Log(LOG_LEVEL_NOMAL, "ip:%s,  running block %d\n", comp_iter->first.c_str(), comp_iter->second.size());

	comp_iter = m_comp_block_map.find(worker);
	if (comp_iter == m_comp_block_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"deleteCompBlock: Can't find CompIP %s in Map\n",worker);
		return ERR_NO_THISITEM;
	}

	CBN_VECTOR& tmpcbn = comp_iter->second;
	size = tmpcbn.size();
	for(tmpi = 0; tmpi < size; tmpi ++){
		
		pBN = tmpcbn[tmpi];
		if (strncmp(pBN->m_guid,blockguid,40) == 0){

			break;
		}

	}

	if (tmpi < size){

		tmpcbn.erase(tmpcbn.begin()+tmpi);
		delete pBN;
		
		if (size == 1){
			m_comp_block_map.erase(comp_iter);
		}

		//�־û�comp��block��ӳ��
		g_Persistence.PersistNoticeMap(worker, tmpcbn);
	}

	return ret;
}

//�޸�ӳ�����block״̬
int CCrackBroker::setCompBlockStatus(const char *worker,char *blockguid,char status){

	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;

	//��������������뵽����ڵ��block ӳ�����
	comp_iter = m_comp_block_map.find(worker);
	if (comp_iter == m_comp_block_map.end()){
		CBN_VECTOR tmpcbn;
		
		pBN = new CBlockNotice();
		if (!pBN){
			
			CLog::Log(LOG_LEVEL_WARNING,"setCompBlockStatus: Create object CBlockNotice <%s,%s> error.\n",worker,blockguid);
			return ERR_OUTOFMEMORY;
		}

		memcpy(pBN->m_guid,blockguid,strlen(blockguid));

		pBN->m_status = status;

		tmpcbn.push_back(pBN);

		m_comp_block_map[worker] = tmpcbn;

		//�־û�comp��block��ӳ��
		g_Persistence.PersistNoticeMap(worker, tmpcbn);

	}else{
		
		CBN_VECTOR& tmpcbn = comp_iter->second;
		size = tmpcbn.size();
		for(tmpi = 0;tmpi < size ;tmpi ++ ){

			pBN = tmpcbn[tmpi];
			if (strncmp(pBN->m_guid,blockguid,40) == 0){

				pBN->m_status = status;
				break;
			}

		}

		if (tmpi == size){

			pBN = new CBlockNotice();
			if (!pBN){
				
				CLog::Log(LOG_LEVEL_WARNING,"setCompBlockStatus: Create object CBlockNotice <%s,%s> error.\n",worker,blockguid);			
				return ERR_OUTOFMEMORY;
			}
			
			memcpy(pBN->m_guid,blockguid,strlen(blockguid));

			pBN->m_status = status;

			tmpcbn.push_back(pBN);

		}

		//�־û�comp��block��ӳ��
		g_Persistence.PersistNoticeMap(worker, tmpcbn);


	}

	return ret;
}

//�õ�ӳ����е��ض�״̬block �б�
int CCrackBroker::getBlockByComp(const char *worker,CBN_VECTOR &cbnvector,char status){

	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;
	CBN_VECTOR::iterator cbn_iter;

	char *p = NULL;

	//��������������뵽����ڵ��block ӳ�����
	comp_iter = m_comp_block_map.find(worker);
	if (comp_iter == m_comp_block_map.end()){
		
		CLog::Log(LOG_LEVEL_DEBUG,"getBlockByComp: %s has no block occupy\n",worker);
		return ERR_NO_THISITEM;

	}else{
		
		CBN_VECTOR& tmpcbn = comp_iter->second;
		cbn_iter = tmpcbn.begin();

		while(cbn_iter != tmpcbn.end()){

			pBN = *cbn_iter;
			
			if (pBN->m_status & status){  //status �����Ƕ��״̬����ϣ����� STATUS_NOTICE_FINISH | STATUS_NOTICE_STOP

				cbnvector.push_back(pBN);
				cbn_iter = tmpcbn.erase(cbn_iter);
			}else{

				cbn_iter++;
			}

		}

		//�־û�
		g_Persistence.PersistNoticeMap(worker, tmpcbn);


		//�ͻ����˳����
		if (status == STATUS_NOTICE_RUN){

			m_comp_block_map.erase(comp_iter);

		}else {

			if( tmpcbn.size() == 0){
				
				m_comp_block_map.erase(comp_iter);

			}	
		}

	}

	return ret;
}


int CCrackBroker::setNoticByHash(CCrackBlock *pCB,int index){

	int ret = 0;
	CCrackTask *pCT = (CCrackTask *)pCB->task;
	CCrackBlock *pTmp = NULL;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end = pCT->m_crackblock_map.end();

	for(iter_block = pCT->m_crackblock_map.begin();iter_block!=iter_block_end;iter_block++){
		
		pTmp = iter_block->second;
		
		
		if ((pTmp->hash_idx == index) && ((pTmp->m_status == WI_STATUS_RUNNING) || (pTmp->m_status == WI_STATUS_LOCK))){

			setCompBlockStatus(pTmp->m_comp_guid,pTmp->guid,STATUS_NOTICE_FINISH);

		}



	}

	return ret;
}

void *CCrackBroker::Alloc(int size){
	
	void *ptr = NULL;
	ptr = malloc(size);
	return ptr;
}

void CCrackBroker::Free(void *p){

	free(p);
	p = NULL;
}

void CCrackBroker::GetTaskFileByGuid(const char* guid, char* fn, int n)
{
	static char path[MAX_PATH] = {0};
	if(path[0] == 0){
		GetModuleFileNameA(NULL, path, MAX_PATH);  
		char *p = strrchr(path, '\\');  
		*p=0x00;  
		
		_snprintf(path, sizeof(path), "%s\\filedb", path);
	}


	if (PathFileExistsA(path) == FALSE){
		int retval = CreateDirectoryA(path,NULL);
		if (retval == 0){
			CLog::Log(LOG_LEVEL_DEBUG,"DIR %s Create Eroor\n",path);
		}else{
			CLog::Log(LOG_LEVEL_DEBUG,"DIR %s Create OK\n",path);
		}
	}

	_snprintf(fn, n, "%s\\%s", path, guid);
}