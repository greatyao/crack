#include "CrackBroker.h"
#include "CompClient.h"


CCrackBroker::CCrackBroker(void)
{
}

CCrackBroker::~CCrackBroker(void)
{
}

//处理登录
int CCrackBroker::ClientLogin(client_login_req *pReq){

	int ret = 0;
	CClientInfo *pCI = NULL;
	time_t tempTm ;

	if(pReq->m_type == COMPUTE_TYPE_CLIENT)
	{
		pCI = new CCompClient;
		((CCompClient *)pCI)->m_gputhreads = pReq->m_gputhreads;
		((CCompClient *)pCI)->m_cputhreads = pReq->m_cputhreads;
	}
	else
		pCI = new CClientInfo;
	pCI->m_clientsock = pReq->m_clientsock;
	pCI->m_type = pReq->m_type;
	memcpy(pCI->m_ip,pReq->m_ip,16);
	memcpy(pCI->m_osinfo,pReq->m_osinfo,sizeof(pCI->m_osinfo));
	memcpy(pCI->m_hostname,pReq->m_hostinfo, sizeof(pCI->m_hostname));
	pCI->m_port = pReq->m_port;
	time(&tempTm);
	pCI->m_logintime = tempTm;

	//m_client_cs.Lock();
	m_client_list.push_back(pCI);
	//m_client_cs.Unlock();

	return ret;
}

//处理心跳
int CCrackBroker::ClientKeepLive2(const char *ip, void* s, unsigned char* cmd, void** data)
{
	unsigned int sock = *(unsigned int *)s;
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

	//不是计算节点，直接发送简单的心跳回应包
	if(pCI == NULL || pCI->m_type != COMPUTE_TYPE_CLIENT)
	{
		*cmd = CMD_REPLAY_HEARTBEAT;
		return 0;
	}


	//获得需要通知的block列表
	getBlockByComp(pCI->m_ip,tmpcbn,STATUS_NOTICE_FINISH | STATUS_NOTICE_STOP);
	int size = tmpcbn.size();


	///////////////////////////////

	//int noneed = 0;//不再需要进行解密的workitem数目，需要根据每个计算节点和block/task之间的关系计算得出
	len = sizeof(keeplive_compclient)+size*sizeof(keeplive_compclient::block_guid);
	ka = (keeplive_compclient*)Alloc(len);
	ka->tasks = m_cracktask_ready_queue.size();
	ka->blocks = size;
	for(int i = 0; i < size; i++)
	{
		//对ka->guids进行赋值
		memset(ka->guids[i],0,40);
		memcpy(ka->guids[i],tmpcbn[i]->m_guid,sizeof(tmpcbn[i]->m_guid));
	
		//回收BlockNotice资源
		delete tmpcbn[i];
	}



	*cmd = CMD_COMP_HEARTBEAT;
	*data = ka;
	return len;
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

	memcpy(pguid,pReq->guid,sizeof(pReq->guid));
	pTask = new CCrackTask;
	ret = pTask->Init(pReq);
	if (ret < 0 ){

		CLog::Log(LOG_LEVEL_WARNING,"Create A New Task Error\n");
		return CREATE_TASK_ERR;

	}
//	m_cracktask_cs.Lock();
	
	m_cracktask_map.insert(CT_MAP::value_type(pTask->guid,pTask));

	for(temp_iter = m_cracktask_map.begin(); temp_iter != m_cracktask_map.end();temp_iter ++ ){
		
		pTask = temp_iter->second;
		CLog::Log(LOG_LEVEL_WARNING,"New task guid=%s, Algo=%d, Charset=%d\n",pTask->guid,pTask->algo,pTask->charset);
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

	m_total_crackblock_map.insert(pCT->m_crackblock_map.begin(),pCT->m_crackblock_map.end());

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
		
		//任务被放入循环队列队尾，等待调度
		if(ret == 0){	//必须确保SetStatus执行成功
			m_cracktask_ready_queue.push_back(pCT->guid);
			CLog::Log(LOG_LEVEL_WARNING,"CrackTask: Set %s Ready and Start\n", pCT->guid);
		}
	}
//	m_cracktask_cs.Unlock();
	
	return ret;
}
	
int CCrackBroker::StopTask(struct task_stop_req *pReq){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end;
	CCrackBlock *pCB = NULL;
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
		
		//将正在运行的block 放入待处理映射中
		iter_block_end = pCT->m_crackblock_map.end();
		for(iter_block = pCT->m_crackblock_map.begin();iter_block!= iter_block_end;iter_block++){
			
			pCB = iter_block->second;
			if ((pCB->m_status == WI_STATUS_RUNNING) || (pCB->m_status == WI_STATUS_LOCK)){
				
				setCompBlockStatus((char *)pCB->m_comp_guid,pCB->guid,STATUS_NOTICE_STOP);

			}

		}
		//////////////////////////////
		
		//Task status --> Running , the block status --> ready
		ret = pCT->SetStatus(CT_STATUS_READY);
		
	}
	
	//如果停止任务成功
	if (ret == 0){

		removeFromQueue(pReq->guid);
		
		


	}
	
	
	//m_cracktask_cs.Unlock();
	return ret;
}

int CCrackBroker::DeleteTask(struct task_delete_req *pReq){

	int ret = 0;
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end;
	CCrackBlock *pCB = NULL;

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
		
		//如果任务正在运行的话，不允许删除
		if (pCT->m_status == CT_STATUS_RUNNING){

			CLog::Log(LOG_LEVEL_WARNING,"Can't Delete Task With GUID %s,Task is Running.\n",pReq->guid);
			return RUN_NOT_DELETE;

		}
		//Task status --> Running , the block status --> ready
		//保守的删除方法，给wi 状态留有余地
	//	ret = pCT->SetStatus(CT_STATUS_DELETED);


		//通知正在运行的block停止运行
		//将正在运行的block 放入待处理映射中
		iter_block_end = pCT->m_crackblock_map.end();
		for(iter_block = pCT->m_crackblock_map.begin();iter_block!= iter_block_end;iter_block++){
			
			pCB = iter_block->second;
			if ((pCB->m_status == WI_STATUS_RUNNING) || (pCB->m_status == WI_STATUS_LOCK)){
				
				setCompBlockStatus((char *)pCB->m_comp_guid,pCB->guid,STATUS_NOTICE_STOP);

			}

		}
		//////////////////////////////


		//暴力删除,直接删除相关任务，及其hash 和crackblock
		this->deleteTask((char *)pReq->guid);
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
		return PAUSE_TASK_ERR;
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

int CCrackBroker::GetTaskResult(struct task_result_req *pReq,struct task_result_info **pRes,int *resNum){

	int ret = 0;
	CT_MAP::iterator iter_task;
	int hashnum = 0;
	CCrackTask *pCT = NULL;
	struct task_result_info *pres = NULL;

//	m_cracktask_cs.Lock();
	
	iter_task = m_cracktask_map.find((char *)pReq->guid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pReq->guid);
		return NOT_FIND_GUID_TASK;
	}else{

		pCT = iter_task->second;

	//	hashnum = pCT->count;
		hashnum = pCT->m_crackhash_list.size();

		pres = (struct task_result_info *)Alloc(sizeof(struct task_result_info)*hashnum);
		if (!pres){
			
			CLog::Log(LOG_LEVEL_WARNING,"Alloc Get Task %s Result Error\n",pReq->guid);
			return ALLOC_TASK_RESULT_ERR;
		}

		//得到计算结果
	//	getResultFromTask(pCT,pres);

		ret = getResultFromTaskNew(pCT,pres);

		*pRes = pres;
		*resNum = ret;
		ret = 0;
	}

//	m_cracktask_cs.Unlock();

	return ret;
}

int CCrackBroker::GetTasksStatus(struct task_status_info **pRes,unsigned int *resNum){
	
	int ret = 0;
	int task_num = 0;
	CT_MAP::iterator iter_task;
	struct task_status_info *pres = NULL;
	CCrackTask *pCT = NULL;
	int j = 0;

	//m_cracktask_cs.Lock();

	//task_num = m_cracktask_queue.size();
	task_num = m_cracktask_map.size();

	pres = (struct task_status_info *)Alloc(sizeof(struct task_status_info)*task_num);
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"Alloc Get Running Task Status Error\n");
	//	m_cracktask_cs.Unlock();
		return ALLOC_TASK_STATUS_ERR;
	}
	
	for(iter_task = m_cracktask_map.begin();iter_task != m_cracktask_map.end();iter_task++){
		
		pCT = iter_task->second;
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
		memcpy(pres[j].hostname,pCI->m_hostname,sizeof(pres[j].hostname));
		memcpy(pres[j].ip,pCI->m_ip,20);
		memcpy(pres[j].os,pCI->m_osinfo,sizeof(pres[j].os));

		pres[j].gputhreads = ((CCompClient *)pCI)->m_gputhreads;
		pres[j].cputhreads = ((CCompClient *)pCI)->m_cputhreads;

		CLog::Log(LOG_LEVEL_WARNING,"CompClient %d: OS=\"%s\" Host=\"%s\" [%d %d]\n", j, pres[j].os, pres[j].hostname,
			pres[j].gputhreads, pres[j].cputhreads );
	
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
	CCrackBlock *pTmpCB = NULL;
	CCrackTask *pCT = NULL;
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;
	char *pguid = NULL;
	int size = 0;

	//CLog::Log(LOG_LEVEL_WARNING,"Enter into CCrackBroker::GetAWorkItem\n");

	pres = (struct crack_block *)Alloc(sizeof(struct crack_block));
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"Alloc crack block error\n");
		return ALLOC_CRACK_BLOCK_ERR;
	}
	
//	m_cracktask_cs.Lock();
	
	memset(pres,0,sizeof(struct crack_block));
	
	size = m_cracktask_ready_queue.size();
	//CLog::Log(LOG_LEVEL_WARNING,"There is %d task Ready\n", size);
	if (size < 1){	
	//	m_cracktask_cs.Unlock();
		return ALLOC_CRACK_BLOCK_ERR;
	}

	pguid = m_cracktask_ready_queue.front();
	//CLog::Log(LOG_LEVEL_WARNING,"Front task guid = %s\n", pguid);
	if (pguid == NULL){

		CLog::Log(LOG_LEVEL_WARNING,"Running Task is Null\n");
//		m_cracktask_cs.Unlock();
		return NO_RUNNING_TASK;
	}

	iter_task = m_cracktask_map.find(pguid);
	if (iter_task == m_cracktask_map.end()){

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pguid);
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(pguid);
		ret =  NOT_FIND_GUID_TASK;
//		m_cracktask_cs.Unlock();
		return ret;
	}

	pCT = iter_task->second;
	
	pCB = pCT->GetAReadyWorkItem();
	if (pCB == NULL){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find A Ready WorkItem from Task %s \n",pguid);
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(pguid);
		return NOT_READY_WORKITEM;
	}
	
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


//新增加的获取block 的函数，添加了对计算节点和block 之间的对应关系
int CCrackBroker::GetAWorkItem2(char *ipinfo,struct crack_block **pRes){

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


	char *pguid = NULL;
	int size = 0;

	//CLog::Log(LOG_LEVEL_WARNING,"Enter into CCrackBroker::GetAWorkItem\n");

	pres = (struct crack_block *)Alloc(sizeof(struct crack_block));
	if (!pres)
	{	
		CLog::Log(LOG_LEVEL_WARNING,"Alloc crack block error\n");
		return ALLOC_CRACK_BLOCK_ERR;
	}
	
//	m_cracktask_cs.Lock();
	
	memset(pres,0,sizeof(struct crack_block));
	
	size = m_cracktask_ready_queue.size();
	//CLog::Log(LOG_LEVEL_WARNING,"There is %d task Ready\n", size);
	if (size < 1){	
	//	m_cracktask_cs.Unlock();
		return ALLOC_CRACK_BLOCK_ERR;
	}

	pguid = m_cracktask_ready_queue.front();
	//CLog::Log(LOG_LEVEL_WARNING,"Front task guid = %s\n", pguid);
	if (pguid == NULL){

		CLog::Log(LOG_LEVEL_WARNING,"Running Task is Null\n");
//		m_cracktask_cs.Unlock();
		return NO_RUNNING_TASK;
	}

	iter_task = m_cracktask_map.find(pguid);
	if (iter_task == m_cracktask_map.end()){

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Task With GUID %s\n",pguid);
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(pguid);
		ret =  NOT_FIND_GUID_TASK;
//		m_cracktask_cs.Unlock();
		return ret;
	}

	pCT = iter_task->second;
	
	pCB = pCT->GetAReadyWorkItem2(ipinfo);
	if (pCB == NULL){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find A Ready WorkItem from Task %s \n",pguid);
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(pguid);
		return NOT_READY_WORKITEM;
	}
	
	

	if (pCT->m_split_num == pCT->m_runing_num){

		m_cracktask_ready_queue.pop_front();

	}else{
	
		m_cracktask_ready_queue.pop_front();
		m_cracktask_ready_queue.push_back(pguid);
	}

	getBlockFromCrackBlock(pCB,pres);

	*pRes = pres;

	///设置block 和comp 对应关系
	//STATUS_NOTICE_RUN
	ret = 0;
	ret = setCompBlockStatus(ipinfo,pCB->guid,STATUS_NOTICE_RUN);
	if (ret < 0 ){
	
		CLog::Log(LOG_LEVEL_WARNING,"Add Computer %s and Block %s Map error\n",ipinfo,pguid);
		
	}
	
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

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Crack Block With GUID %s\n", block_guid);
		return  NOT_FIND_GUID_BLOCK;
	}

	pCB = iter_block->second;
	pCT =(CCrackTask *)pCB->task;
	if(!pCT) return NOT_FIND_GUID_BLOCK;
	
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

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Crack Block With GUID %s\n",pReq->guid);
		ret =  NOT_FIND_GUID_BLOCK;
		return ret;
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

		CLog::Log(LOG_LEVEL_WARNING,"Can't find Crack Block With GUID %s\n",pReq->guid);
		ret =  NOT_FIND_GUID_BLOCK;
		return ret;
	}
	
	pCB = iter_block->second;
	int index = pCB->hash_idx;
	switch(pReq->status){

		case WI_STATUS_UNLOCK:
			CLog::Log(LOG_LEVEL_WARNING, "**** Reuse WorkItem [guid=%s] ****\n",pReq->guid);
			
			pCB->m_status = WI_STATUS_READY;
			((CCrackTask *)(pCB->task))->m_runing_num -=1;

			//将任务就绪队列更新
			updateReadyQueue(pCB);

			//block占用释放，从block--computer 映射中删除
			deleteCompBlock((char *)pCB->m_comp_guid,pCB->guid);

			break;
		case WI_STATUS_RUNNING:

			pCB->m_status = WI_STATUS_RUNNING;

			//判断是否是第一个开始运行的Block ,如果是的话更新任务计时器
			pCT = (CCrackTask *)pCB->task;
			pCT->startTime();				


			checkReadyQueue((CCrackTask *)(pCB->task));
			break;
		case WI_STATUS_CRACKED:
			CLog::Log(LOG_LEVEL_WARNING, "**** WorkItem [guid=%s] password=\"%s\" ****\n",pReq->guid,pReq->password);
			pCB->m_status = WI_STATUS_CRACKED;
			pCT =(CCrackTask *)pCB->task;
						
			//解密成功，同一个hash下的block将不需要继续计算
			setNoticByHash(pCB,index);
			//删除完成block
			deleteCompBlock((char *)pCB->m_comp_guid,pCB->guid);


		//	m_cracktask_cs.Lock();
			ret = pCT->updateStatusToFinish(pReq,index);
			if (ret == 1){

				removeFromQueue((unsigned char *)pCT->guid);
			}

		//	m_cracktask_cs.Unlock();
			break;
		case WI_STATUS_NO_PWD:
			CLog::Log(LOG_LEVEL_WARNING, "**** WorkItem [guid=%s] NON password ****\n",pReq->guid);
			
			pCB->m_status = WI_STATUS_NO_PWD;
			pCT = (CCrackTask *)pCB->task;
		//	m_cracktask_cs.Lock();

			//删除完成block
			deleteCompBlock((char *)pCB->m_comp_guid,pCB->guid);


			ret = pCT->updateStatusToFinish(pReq,index);

			if (ret == 1){

				removeFromQueue((unsigned char *)pReq->guid);
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



int CCrackBroker::removeFromQueue(unsigned char *guid){

	int ret = 0;
	CT_DEQUE::iterator iter_queue;

	//从调度队列中移除
	/*	
	for(iter_queue = m_cracktask_queue.begin();iter_queue != m_cracktask_queue.end(); iter_queue ++ ){
	
		if (strncmp(*iter_queue,(char *)guid,40) == 0 ){		
			break;
		}
	}
	if (iter_queue != m_cracktask_queue.end()){
			m_cracktask_queue.erase(iter_queue);
	}
	*/

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


//老的任务结果返回处理
int CCrackBroker::getResultFromTask(CCrackTask *pCT,struct task_status_res *pRes){
	
	int ret =0;

	pRes->status = pCT->m_status;
	memcpy(pRes->guid,pCT->guid,40);
	memcpy(pRes->password,pCT->m_result,32);

	return ret;
}


//新的任务结果返回处理
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

	return ret;  //返回结果数目
}
	
int CCrackBroker::getStatusFromTask(CCrackTask *pCT,task_status_info *pRes){

	int ret = 0;
	time_t mytime = time(NULL);

	pRes->m_fini_number = pCT->m_finish_num;
	pRes->m_split_number = pCT->m_split_num;

	pRes->m_remain_time = pCT->m_remain_time;

	pRes->m_running_time = pCT->m_running_time;

	if (pCT->m_status == CT_STATUS_RUNNING){

		pRes->m_running_time = (pCT->m_running_time + (mytime-pCT->m_start_time));


	}

	pRes->m_algo = pCT->algo;
	//get the current block progress 
	pCT->calcProgressByBlock();
	pRes->m_progress = pCT->m_progress;
	pRes->m_speed = pCT->m_speed;
	pRes->status = pCT->m_status;
	memcpy(pRes->guid,pCT->guid,40);
	return ret;
}

int CCrackBroker::getBlockFromCrackBlock(CCrackBlock *pCB,struct crack_block *pRes){
	
	int ret = 0;
	crack_block* parent = pCB;
	memcpy(pRes, parent, sizeof(crack_block));
	pRes->task = pCB->task;
	
	return ret;
}

int CCrackBroker::DoClientQuit(char *ip,int port){
	
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

	//计算节点退出的话还需要，将计算节点正在计算的block状态出始化
	getBlockByComp(ip,tmpcbn,STATUS_NOTICE_RUN);

	if (tmpcbn.size() == 0){

		
		return 0;
	}
	
	//将所有相关block 设置状态为ready,进行重新分配，进度信息清空
	size = tmpcbn.size();
	for(i = 0;i < size ;i ++ ){
		
		iter_block = m_total_crackblock_map.find(tmpcbn[i]->m_guid);

		//释放资源
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

		//将相关任务放入就绪队列
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
		
		if (strcmp((char *)m_cracktask_ready_queue[i],pCT->guid) == 0){

			break;
		}

	}

	if (i == size){

		m_cracktask_ready_queue.push_back(pCT->guid);
	}

}

int CCrackBroker::deleteTask(char *guid){

	int ret = 0;
	CCrackTask *pCT = NULL;
	CCrackHash *pCH = NULL;
	CCrackBlock *pCB = NULL;
	CRACK_HASH_LIST::iterator iter_hash;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_total_block;
	CB_MAP::iterator total_block_end;
	CB_MAP tmp_cb_map;

	total_block_end = m_total_crackblock_map.end();

	CT_MAP::iterator iter_task = m_cracktask_map.find(guid);
	if (iter_task == m_cracktask_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"Delete Task ,Can't find Crack Task With GUID %s\n",guid);
		ret =  NOT_FIND_GUID_TASK;
		return ret;
	}

	pCT=iter_task->second;
	//删除hash 中的内容
	if (pCT->m_crackhash_list.size() > 0){

		iter_hash = pCT->m_crackhash_list.begin();
		
		pCH = *iter_hash;

		delete []pCH;
		pCT->m_crackhash_list.clear();
		pCT->m_crackhash_list.clear();
	}

	//删除crackblock 内容
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
			
			if (strcmp(pCT->guid,m_cracktask_ready_queue[i]) == 0){
					
				break;

			}

		}

		if (i < m_cracktask_ready_queue.size()){

			m_cracktask_ready_queue.erase(m_cracktask_ready_queue.begin()+i);
		}
	//	m_cracktask_ready_queue.erase(pCT->guid);

	}


}



//添加新的block 
/*
int CCrackBroker::addNewCompBlock(char *ipinfo,char *blockguid,char status){
	
	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;
	CBN_VECTOR tmpcbn;

	//被锁定的任务加入到计算节点和block 映射表中
	comp_iter = m_comp_block_map.find(ipinfo);
	if (comp_iter == m_comp_block_map.end()){
		
		pBN = new CBlockNotice();
		if (!pBN){
			
			CLog::Log(LOG_LEVEL_WARNING,"Create BlockNotice Error.CompIP : %s,Block : %s\n",ipinfo,blockguid);
			ret = -10;
			return ret;
		}
		
		memset(pBN->m_guid,0,40);
		memcpy(pBN->m_guid,blockguid,strlen(blockguid));

		pBN->m_status = status;

		tmpcbn.push_back(pBN);

		m_comp_block_map.insert(CCB_MAP::value_type(ipinfo,tmpcbn));

	}else{
		
		tmpcbn = comp_iter->second;
		size = tmpcbn.size();
		for(tmpi = 0;tmpi < size ;tmpi ++ ){

			pBN = tmpcbn[tmpi];
			if (strcmp(pBN->m_guid,blockguid,40) == 0){

				pBN->m_status = status;
				break;
			}

		}

		if (tmpi == size){

			pBN = new CBlockNotice();
			if (!pBN){
				
				CLog::Log(LOG_LEVEL_WARNING,"Create BlockNotice Error.CompIP : %s,Block : %s\n",ipinfo,blockguid);
				ret = -10;
				return ret;
			}
			
			memset(pBN->m_guid,0,40);
			memcpy(pBN->m_guid,blockguid,strlen(blockguid));

			pBN->m_status = status;

			tmpcbn.push_back(pBN);

		}

	}

	return ret;
}

*/
//从映射表中删除block
int CCrackBroker::deleteCompBlock(char *ipinfo,char *blockguid){

	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;
	CBN_VECTOR tmpcbn;

	//被锁定的任务加入到计算节点和block 映射表中
	comp_iter = m_comp_block_map.find(ipinfo);
	if (comp_iter == m_comp_block_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"Can't find CompIP : %s in Map\n",ipinfo);
		ret = -2;
		return ret;
	}

	tmpcbn = comp_iter->second;
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
	}

	return ret;
}

//修改映射表中block状态
int CCrackBroker::setCompBlockStatus(char *ipinfo,char *blockguid,char status){

	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;
	CBN_VECTOR tmpcbn;

	//被锁定的任务加入到计算节点和block 映射表中
	comp_iter = m_comp_block_map.find(ipinfo);
	if (comp_iter == m_comp_block_map.end()){
		
		pBN = new CBlockNotice();
		if (!pBN){
			
			CLog::Log(LOG_LEVEL_WARNING,"Create BlockNotice Error.CompIP : %s,Block : %s\n",ipinfo,blockguid);
			ret = -10;
			return ret;
		}
		
		memset(pBN->m_guid,0,40);
		memcpy(pBN->m_guid,blockguid,strlen(blockguid));

		pBN->m_status = status;

		tmpcbn.push_back(pBN);

		m_comp_block_map.insert(CCB_MAP::value_type(ipinfo,tmpcbn));

	}else{
		
		tmpcbn = comp_iter->second;
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
				
				CLog::Log(LOG_LEVEL_WARNING,"Create BlockNotice Error.CompIP : %s,Block : %s\n",ipinfo,blockguid);
				ret = -10;
				return ret;
			}
			
			memset(pBN->m_guid,0,40);
			memcpy(pBN->m_guid,blockguid,strlen(blockguid));

			pBN->m_status = status;

			tmpcbn.push_back(pBN);

		}

	}

	return ret;
}


//得到映射表中的特定状态block 列表
int CCrackBroker::getBlockByComp(char *ipinfo,CBN_VECTOR cbnvector,char status){

	int ret =0;
	int size = 0;
	int tmpi = 0;
	CCB_MAP::iterator comp_iter;
	CBlockNotice *pBN = NULL;
	CBN_VECTOR::iterator cbn_iter;
	CBN_VECTOR tmpcbn;

	char *p = NULL;

	//被锁定的任务加入到计算节点和block 映射表中
	comp_iter = m_comp_block_map.find(ipinfo);
	if (comp_iter == m_comp_block_map.end()){
		
		CLog::Log(LOG_LEVEL_WARNING,"Compute Node : %s has no block occupy\n",ipinfo);
		return ret;

	}else{
		
		tmpcbn = comp_iter->second;
		cbn_iter = tmpcbn.begin();

		while(cbn_iter != tmpcbn.end()){

			pBN = *cbn_iter;
			
			if (pBN->m_status & status){  //status 可能是多个状态的组合，例如 STATUS_NOTICE_FINISH | STATUS_NOTICE_STOP

				cbnvector.push_back(pBN);
				tmpcbn.erase(cbn_iter++);
			}else{

				cbn_iter++;
			}

		}


		//客户端退出情况
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
	CB_MAP::iterator iter_block_end;

	for(iter_block = pCT->m_crackblock_map.begin();iter_block!=iter_block_end;iter_block++){
		
		pTmp = iter_block->second;
		
		
		if ((pTmp->hash_idx == index) && ((pTmp->m_status == WI_STATUS_RUNNING) || (pTmp->m_status == WI_STATUS_LOCK))){

			setCompBlockStatus((char *)pTmp->m_comp_guid,pTmp->guid,STATUS_NOTICE_FINISH);

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