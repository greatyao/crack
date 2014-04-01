#include "CrackBroker.h"
#include "crack_status.h"

#include "CompClient.h"


CCrackBroker::CCrackBroker(void)
{
}

CCrackBroker::~CCrackBroker(void)
{
}




//处理登录
int CCrackBroker::ClientLogin(struct client_login_req *pReq){

	int ret = 0;
	CClientInfo *pCI = NULL;
	time_t tempTm ;

	pCI = new CClientInfo;
	pCI->m_clientsock = pReq->m_clientsock;
	pCI->m_type = pReq->m_type;
	//memcpy(pCI->m_guid,pReq->m_guid,40);
	memcpy(pCI->m_ip,pReq->m_ip,20);
	memcpy(pCI->m_osinfo,pReq->m_osinfo,16);
	time(&tempTm);
	pCI->m_logintime = tempTm;

	//m_client_cs.Lock();
	m_client_list.push_back(pCI);
	//m_client_cs.Unlock();

	/*switch(pReq->m_type){

		case COMPUTE_TYPE_CLIENT:
			
			pCI = new CCompClient;
			pCI->m_clientsock = pReq->m_clientsock;
			pCI->m_/pe = pReq->m_type;
			memcpy(pCI->m_guid,pReq->m_guid,40);
			memcpy(pCI->m_ip,pReq->m_ip,20);
			memcpy

			break;
		case CONTROL_TYPE_CLIENT:


			break;
		default:
	
			ret = -1;
			break;

	}
	
*/


	return ret;

}

//处理心跳
int CCrackBroker::ClientKeepLive(char *ip){
	
	int ret = 0;
	int i = 0;
	CClientInfo *pCI = NULL;
	time_t temptm;

	time(&temptm);
	//m_client_cs.Lock();

	for (i = 0 ;i < m_client_list.size(); i ++){

			pCI= m_client_list[i];
			if (memcmp(pCI->m_ip,ip,16) == 0){

				pCI->m_keeplivetime = temptm;
				break;
			}
	}

	
//	m_client_cs.Unlock();
	return ret;
}

//控制节点业务逻辑处理函数

//创建新任务
int	CCrackBroker::CreateTask(struct crack_task *pReq,unsigned char *pguid){

	int ret = 0;
	CCrackTask *pTask = NULL;
	CT_MAP::iterator temp_iter;


	pTask = new CCrackTask;
	ret = pTask->Init(pReq);
	if (ret < 0 ){

		CLog::Log(LOG_LEVEL_WARNING,"Create A New Task Error\n");
		return CREATE_TASK_ERR;

	}
//	m_cracktask_cs.Lock();
	
	m_cracktask_map.insert(CT_MAP::value_type(pTask->guid,pTask));

	m_total_crackblock_map.insert(pTask->m_crackblock_map.begin(),pTask->m_crackblock_map.end());
	
	memcpy(pguid,pTask->guid,40);


	for(temp_iter = m_cracktask_map.begin(); temp_iter != m_cracktask_map.end();temp_iter ++ ){
		
			
		CLog::Log(LOG_LEVEL_WARNING,"Task guid : %s, Task : %d, %d, %s\n",pTask->guid,pTask->algo,pTask->charset,pTask->filename);
		

	}

//	m_cracktask_cs.Unlock();
	return ret;

}


//切分任务接口
int CCrackBroker::SplitTask(char *pguid){
	
	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
		
	iter_task = m_cracktask_map.find(pguid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pguid);
		return NOT_FIND_GUID_TASK;
	}

	pCT = iter_task->second;
	
	ret = pCT->SplitTaskFile(pguid);
	if (ret < 0 ){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task GUID %s ,Split Error\n",pguid);
		return TASK_SPLIT_ERR;
		
	}


	return ret;
}


//开始新任务
int	CCrackBroker::StartTask(struct task_start_req *pReq){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;

	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"Start Task Req NULL Error\n");
		return START_TASK_ERR;

	}
	
//	m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pReq->guid);
		return NOT_FIND_GUID_TASK;
	}else{
		
		pCT = iter_task->second;
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_RUNNING);

	}
	m_cracktask_queue.push_back(pCT->guid);  //任务被放入循环队列队尾，等待调度

	m_cracktask_ready_queue.push_back(pCT->guid);

	CLog::Log(LOG_LEVEL_WARNING,"CrackTask :%d %d %s %d %s %d %d\n",pCT->algo,pCT->charset,pCT->filename,pCT->type,pCT->guid,pCT->startLength,pCT->endLength);
//	m_cracktask_cs.Unlock();
	
	return ret;
}
	
int CCrackBroker::StopTask(struct task_stop_req *pReq){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;

	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"Stop Task Req NULL Error\n");
		return STOP_TASK_ERR;

	}
	
	//m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pReq->guid);
		return NOT_FIND_GUID_TASK;
	}else{
		
		pCT = iter_task->second;
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_READY);

	}
	
	removeFromQueue(pReq->guid);
	
	//m_cracktask_cs.Unlock();
	return ret;

}
int CCrackBroker::DeleteTask(struct task_delete_req *pReq){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;

	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"Delete Task Req NULL Error\n");
		return DEL_TASK_ERR;

	}
	
//	m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pReq->guid);
		return NOT_FIND_GUID_TASK;

	}else{
		
		pCT = iter_task->second;
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_DELETED);

	}

	//从调度队列中移除
	removeFromQueue(pReq->guid);
	

	//m_cracktask_cs.Unlock();
	return ret;

}

int CCrackBroker::PauseTask(struct task_pause_req *pReq){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;

	if (!pReq){

		CLog::Log(LOG_LEVEL_WARNING,"Pause Task Req NULL Error\n");
		return DEL_TASK_ERR;

	}
	
//	m_cracktask_cs.Lock();

	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
			
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pReq->guid);
		return NOT_FIND_GUID_TASK;

	}else{
		
		pCT = iter_task->second;
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_PAUSED);

	}
	
	//从调度队列中移除
	 removeFromQueue(pReq->guid);
	
//	m_cracktask_cs.Unlock();
	return ret;

}
int CCrackBroker::GetTaskResult(struct task_result_req *pReq,struct task_status_res **pRes){

	int ret = 0;
	CT_MAP::iterator iter_task;
	
	CCrackTask *pCT = NULL;
	struct task_status_res *pres = NULL;

	pres = (struct task_status_res *)Alloc(sizeof(struct task_status_res));
	if (!pres){
		
			
		CLog::Log(LOG_LEVEL_WARNING,"Alloc Get Task %s Result Error\n",pReq->guid);
		return ALLOC_TASK_RESULT_ERR;

	}
	
//	m_cracktask_cs.Lock();
	
	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pReq->guid);
		return NOT_FIND_GUID_TASK;
	}else{

		
		pCT = iter_task->second;

		//得到计算结果
		getResultFromTask(pCT,pres);

		*pRes = pres;
		ret = 0;

	}


//	m_cracktask_cs.Unlock();

	return ret;


}

int CCrackBroker::GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum){
	
	int ret = 0;
	int task_num = 0;
	CT_MAP::iterator iter_task;
	CT_DEQUE::iterator iter_deque;
	struct task_status_info *pres = NULL;
	CCrackTask *pCT = NULL;
	char *pCh = NULL;
	int i = 0;
	int j = 0;

	//m_cracktask_cs.Lock();

	task_num = m_cracktask_queue.size();
	
	pres = (struct task_status_info *)Alloc(sizeof(struct task_status_info)*task_num);
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"Alloc Get Running Task Status Error\n");
	//	m_cracktask_cs.Unlock();
		return ALLOC_TASK_STATUS_ERR;
	}
	
	for(i=0 ;i < task_num ;i ++){

		
		pCh = m_cracktask_queue[i];

		iter_task = m_cracktask_map.find((char *)pCh);
		if (iter_task == m_cracktask_map.end()){

			CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pCh);
			Free(pres);
	//		m_cracktask_cs.Unlock();
			return NOT_FIND_GUID_TASK;
			
		}
		

		pCT = iter_task->second;

		//从CCrackTask 得到 Status info
		getStatusFromTask(pCT,&pres[j]);
		j++;

	}


	
	*pRes = pres;
	*resNum = j;
	//m_cracktask_cs.Unlock();

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
		CLog::Log(LOG_LEVEL_WARNING,"Alloc Computer Client Error\n");
	//	m_client_cs.Unlock();
		return ALLOC_COMP_CLIENT_ERR;
	}
	
	memset(pres,0,sizeof(struct compute_node_info) * client_num);

	for(i=0 ;i < size ;i++){

		pCI = m_client_list[i];

		if (pCI->m_type != COMPUTE_TYPE_CLIENT)
			continue;


		//从CClientInfo 得到 computer_node_info 
		
		memcpy(pres[j].guid,pCI->m_guid,40);
		memcpy(pres[j].hostname,pCI->m_hostname,50);
		memcpy(pres[j].ip,pCI->m_ip,20);
		memcpy(pres[j].os,pCI->m_osinfo,16);

		
		pres[j].gputhreads = ((CCompClient *)pCI)->m_gputhreads;
		pres[j].cputhreads = ((CCompClient *)pCI)->m_cputhreads;

	
		j++;
	}
	
	*pRes = pres;
	*resNum = j;

//	m_client_cs.Unlock();
	return ret;

}

	
//计算节点业务逻辑处理函数
int CCrackBroker::GetAWorkItem(struct crack_block **pRes){

	int ret = 0;
	CT_MAP::iterator iter_task;
	struct crack_block *pres = NULL;
	CCrackTask *pCT = NULL;
	CCrackBlock *pCB = NULL;
	char *pguid = NULL;
	int size = 0;

	CLog::Log(LOG_LEVEL_WARNING,"Enter into Get A Work Item\n");

	pres = (struct crack_block *)Alloc(sizeof(struct crack_block));
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"Alloc crack block error\n");
		return ALLOC_CRACK_BLOCK_ERR;
	}
	
//	m_cracktask_cs.Lock();
	
	
	size = m_cracktask_ready_queue.size();
	if (size < 1){
		
		CLog::Log(LOG_LEVEL_WARNING,"There isnot a Task Ready\n");
	//	m_cracktask_cs.Unlock();
		return ALLOC_CRACK_BLOCK_ERR;
	}

	pguid = m_cracktask_ready_queue.front();
	if (pguid == NULL){

		CLog::Log(LOG_LEVEL_WARNING,"Running Task is Null\n");
//		m_cracktask_cs.Unlock();
		return NO_RUNNING_TASK;

	}


	iter_task = m_cracktask_map.find(pguid);
	if (iter_task == m_cracktask_map.end()){

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pguid);
		ret =  NOT_FIND_GUID_TASK;
//		m_cracktask_cs.Unlock();
		return ret;

	}

	pCT = iter_task->second;
	
	pCB = pCT->GetAReadyWorkItem();
	
	if (pCT->m_split_num == pCT->m_runing_num){

		m_cracktask_ready_queue.pop_front();


	}else{
	
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(pguid);

	}

	getBlockFromCrackBlock(pCB,pres);

	*pRes = pres;
//	m_cracktask_cs.Unlock();

	return ret;

}

int CCrackBroker::GetWIStatus(struct crack_status *pReq){

	int ret = 0;
	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;


	iter_block = m_total_crackblock_map.find(pReq->guid);
	if (iter_block == m_total_crackblock_map.end()){

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Crack Block With GUID %s\n",pReq->guid);
		ret =  NOT_FIND_GUID_BLOCK;
		return ret;


	}
	
	pCB = iter_block->second;

	pCB->m_progress = pReq->progress;
	pCB->m_speed = pReq->speed;
	pCB->m_remaintime = pReq->remainTime;
	return ret;

}
/*
#define WORK_ITEM_AVAILABLE		0	//workitem的起始状态，可供其他计算单元使用
#define WORK_ITEM_LOCK			1	//workitem已经被一个计算单元占用，但是不确定计算单元的解密任务是否进行，此时不能被其他计算单元使用
#define WORK_ITEM_UNLOCK		2	//计算单元通知服务端unlock该资源，重新设置为avaiable，以供其他计算节点使用
#define WORK_ITEM_WORKING		3	//计算单元正在对该workitem进行解密任务
#define WORK_ITEM_CRACKED		4	//计算单元完成解密任务，同时破解出密码
#define WORK_ITEM_UNCRACKED		5	//计算单元完成解密任务，但没有破解出密码
*/

int CCrackBroker::GetWIResult(struct crack_result *pReq){

	int ret = 0;
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;
	CCrackTask *pCT = NULL;

	iter_block = m_total_crackblock_map.find(pReq->guid);
	if (iter_block == m_total_crackblock_map.end()){

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Crack Block With GUID %s\n",pReq->guid);
		ret =  NOT_FIND_GUID_BLOCK;
		return ret;
	}
	
	pCB = iter_block->second;
	
	switch(pReq->status){

		case WORK_ITEM_LOCK:
			

			break;
		case WORK_ITEM_UNLOCK:
			
			pCB->m_status = WI_STATUS_READY;

			break;
		case WORK_ITEM_WORKING:


			break;
		case WORK_ITEM_CRACKED:

			pCB->m_status = WI_STATUS_FINISHED;
			pCT =(CCrackTask *)pCB->task;
		//	m_cracktask_cs.Lock();
			
		//	removeFromQueue((unsigned char *)pCT->guid);
			

		//	pCT->updateStatusToFinish(pReq);

		//	m_cracktask_cs.Unlock();
			break;
		case WORK_ITEM_UNCRACKED:

			pCB->m_status = WI_STATUS_NO_PWD;
			pCT = (CCrackTask *)pCB->task;
		//	m_cracktask_cs.Lock();

			ret = pCT->updateStatusToFinish(pReq);

			if (ret == 1){

				removeFromQueue((unsigned char *)pReq->guid);

			}

		//	m_cracktask_cs.Unlock();
			break;
		default:
			
			pCB->m_status = WI_STATUS_FAILURE;
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



int CCrackBroker::removeFromQueue(unsigned char *guid){

	int ret = 0;
	CT_DEQUE::iterator iter_queue;

	//从调度队列中移除
	for(iter_queue = m_cracktask_queue.begin();iter_queue != m_cracktask_queue.end(); iter_queue ++ ){
	
		if (strncmp(*iter_queue,(char *)guid,40) == 0 ){
			
			break;

		}

	}

	if (iter_queue != m_cracktask_queue.end()){
			m_cracktask_queue.erase(iter_queue);
	}



	//从ready 队列中移除

	for(iter_queue = m_cracktask_ready_queue.begin();iter_queue != m_cracktask_ready_queue.end(); iter_queue ++ ){
	
		if (strncmp(*iter_queue,(char *)guid,40) == 0 ){
			
			break;

		}

	}

	if (iter_queue != m_cracktask_ready_queue.end()){
			m_cracktask_ready_queue.erase(iter_queue);
	}

	return ret;
}


int CCrackBroker::getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes){
	
	int ret =0;

	pRes->status = pCT->m_status;
	memcpy(pRes->guid,pCT->guid,40);
	memcpy(pRes->password,pCT->m_result,32);

	return ret;
}
	
int CCrackBroker::getStatusFromTask(CCrackTask *pCT,struct task_status_info *pRes){

	int ret = 0;

	pRes->m_fini_number = pCT->m_finish_num;
	pRes->m_split_number = pCT->m_split_num;
	pRes->m_progress = pCT->m_progress;
	pRes->status = pCT->m_status;
	memcpy(pRes->guid,pCT->guid,40);
	return ret;
}

int CCrackBroker::getBlockFromCrackBlock(CCrackBlock *pCB,struct crack_block *pRes){
	
	int ret = 0;
	
	pRes->algo = pCB->algo;
	pRes->charset = pCB->charset;
	pRes->end = pCB->end;
	pRes->end2 = pCB->end2;
	memcpy(pRes->guid,pCB->guid,40);
	memcpy(pRes->john,pCB->john,sizeof(struct crack_hash));
	
	pRes->special = pCB->special;
	pRes->start = pCB->start;
	pRes->start2 = pCB->start2;
	pRes->type = pCB->type;
	pRes->task = pCB->task;

	
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


