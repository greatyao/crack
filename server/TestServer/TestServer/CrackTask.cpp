
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "split.h"
#include "macros.h"
#include "err.h"
#include "loadfiles.h"
#include "CLog.h"

#include "CrackTask.h"
#include "CrackHash.h"
#include "CrackBlock.h"
#include "CrackBroker.h"
#include "BlockNotice.h"
#include "PersistencManager.h"

CCrackTask::CCrackTask(void)
{
}

CCrackTask::~CCrackTask(void)
{
}

int CCrackTask::Init(crack_task *pCrackTask)
{
	//直接按照结构体赋值拷贝一大串内存即可，无需一个个依次赋值
	crack_task* parent = this;
	memcpy(parent, pCrackTask, sizeof(crack_task));

	m_status = CT_STATUS_READY;   //状态准备运行
	
	resetProgress();  //进度信息设置为初始状态
	memset(m_control_guid,0,40);

	m_priority = CT_PRIORITY_NORMAL;

	m_file = NULL;

	m_start_time = time(NULL);
	m_speed = 0;
	m_running_time = 0;
	m_remain_time = 0;

	return 0;
}


int CCrackTask::SplitTaskFile(const char *guid, const char* john){

	int ret = 0;
	CCrackBlock *pCb = NULL;
	CCrackHash *pCCH = NULL;
	csplit split;
	struct crack_hash *pch = NULL;
	crack_block *pCrackBlock = NULL;
	unsigned int splitnum = 0;
	void *p = NULL;
	int mcount = 0;
	int i = 0;

	memcpy(this->guid, guid,40);

	count = 0;

	//调用文件分割函数
	if(john == NULL){
		CCrackBroker::GetTaskFileByGuid(guid, (char*)filename, sizeof(filename));

		if(this->algo == algo_msoffice || this->algo == algo_msoffice_old)
			this->algo = algo_msoffice;

		ret = load_hashes_file2((char *)filename,this);

		if(this->algo == algo_msoffice && ret <= 0)
		{
			this->algo = algo_msoffice_old;
			ret = load_hashes_file2((char *)filename,this);
		}

	} else{
		ret = load_single_hash2((char *)john, this);
	}

	if (ret <= 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"SplitTaskFile: load the file Error\n");
		return john == NULL ? ERR_WRONGFILE : ERR_WRONGHASHES;
	}
	
	pCCH = new (std::nothrow)CCrackHash[count];
	if(pCCH == NULL){
		CLog::Log(LOG_LEVEL_ERROR, "SplitTaskFile: Alloc %d CrackHash object error\n", count);
		return  ERR_OUTOFMEMORY;
	}

	for (i = 0 ;i < ret ;i ++ ){

		pCCH[i].Init((unsigned char *)this->hashes[i].hash);

		m_crackhash_list.push_back(&pCCH[i]);

		if(this->special == 0)
			CLog::Log(LOG_LEVEL_DEBUG, "SplitTaskFile: Hash is %s,%s,%s\n",hashes[i].hash,hashes[i].salt,hashes[i].salt2);
	}

	//出始化相关工作项
	pCrackBlock = split.split_task(this,splitnum);
	if (!pCrackBlock){
		CLog::Log(LOG_LEVEL_WARNING,"SplitTaskFile: split_task Error\n");
		release_hashes_from_load(this);
		return ERR_SPLITTASK;
	}
	CLog::Log(LOG_LEVEL_NOMAL,"SplitTaskFile: Wonderful! We split task by %d blocks\n", splitnum);		
	
	//根据crack_block 创建 CCrackBlock 对象
	pCb = new (std::nothrow)CCrackBlock[splitnum];
	if(pCb == NULL){
		release_hashes_from_load(this);
		split.release_splits((char *)pCrackBlock);
		CLog::Log(LOG_LEVEL_ERROR, "SplitTaskFile: Alloca %d CrackBlock object error\n", splitnum);
		return  ERR_OUTOFMEMORY;
	}
	for (int i = 0; i < splitnum;i ++ ){
		
		pCb[i].Init(&pCrackBlock[i]);
		pCb[i].task = this;
		
		m_crackblock_map.insert(CB_MAP::value_type(pCb[i].guid,&pCb[i]));
		if(this->special == 0)
		{
			if(pCb[i].type == bruteforce)
				CLog::Log(LOG_LEVEL_NOMAL,"item guid=%s [%d,%d] algo=%d\n",pCb[i].guid, pCb[i].start, pCb[i].end, pCb[i].algo);
			else if(pCb[i].type == dict)
				CLog::Log(LOG_LEVEL_NOMAL,"item guid=%s [dict=%d] algo=%d\n",pCb[i].guid, pCb[i].dict_idx, pCb[i].algo);
			if(pCb[i].type == mask && pCb[i].flag == 0)
				CLog::Log(LOG_LEVEL_NOMAL,"item guid=%s [%d, %s] algo=%d\n",pCb[i].guid, pCb[i].maskLength, pCb[i].masks, pCb[i].algo);
		}
	}

	//释放资源
	release_hashes_from_load(this);
	this->hashes = NULL;
	split.release_splits((char *)pCrackBlock);
	this->m_split_num = splitnum;
	this->m_finish_num = 0;
	count = ret;
	ret = 0;
	return ret;
}

//新增加的获取block 函数，增加了对block 中的comp_guid 的赋值
CCrackBlock *CCrackTask::GetAReadyWorkItem2(const char *owner){

	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end = m_crackblock_map.end();
	CB_MAP::iterator iter_block_begin = m_crackblock_map.begin();

	iter_block = cur_crack_block;
	do{
		if (iter_block->second->m_status == WI_STATUS_READY){

			pCB = iter_block->second;
	//		pCB->m_status = WI_STATUS_RUNNING;
			pCB->m_status = WI_STATUS_LOCK;  //工作项首先被计算节点锁定，然后根据处理状态返回Unlock,Running
			m_runing_num ++;

			//将计算节点的信息赋值给block,ipinfo内容为ip:port
			memcpy(pCB->m_comp_guid,owner,strlen(owner));

			
			break;
		}

		iter_block++;
		if(iter_block == iter_block_end)
			iter_block = iter_block_begin;

	}while(iter_block != cur_crack_block);

	cur_crack_block = ++iter_block;
	if(cur_crack_block == iter_block_end)
		cur_crack_block = iter_block_begin;
	
	return pCB;

}


void CCrackTask::calcProgressByBlock(){

	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;
	float fprogress = 0.0;
	float speed = 0;

	for (iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){

		pCB = iter_block->second;
		if (pCB->m_status == WI_STATUS_RUNNING){
			
			fprogress += pCB->m_progress;
			if(speed < pCB->m_speed)
				speed = pCB->m_speed;
		}else if ((pCB->m_status == WI_STATUS_WAITING) || (pCB->m_status == WI_STATUS_READY )||
			(pCB->m_status == WI_STATUS_LOCK)||(pCB->m_status == WI_STATUS_UNLOCK)){
			
			fprogress +=0;

		}else {

			fprogress +=100;  //设置了其他的状态的进度按照100来计算
		}

	}

	int blocknum = m_crackblock_map.size();
	this->m_progress = blocknum == 0 ? 0 :fprogress/blocknum;	
	this->m_speed = speed;

}

int CCrackTask::SetStatus(char status){

	int ret = 0;
	
	switch(status){
		
		case CT_STATUS_RUNNING:
				
			ret = updateStatusToRunning();
		
			break;
		case CT_STATUS_READY:

			ret = updateStatusToStop();

			break;
		case CT_STATUS_PAUSED:
			
			ret = updateStatusToPause();

			break;

		case CT_STATUS_DELETED:

			ret = updateStatusToDel();

			break;

		default :

			CLog::Log(LOG_LEVEL_WARNING,"Not Support Crack Task Status %d \n",status);
			ret = ERR_NOSUPPORTSTATUS;
			break;

	}
	//set the block status


	
	return ret;
}

/*
typedef enum CRACK_TASK_STATUS {

	CT_STATUS_READY = 1,   //任务的就绪状态
	CT_STATUS_FINISHED,		//任务完成状态	
	CT_STATUS_FAILURE,		//任务解密失败状态
	CT_STATUS_RUNNING,		//任务正在解密状态

	CT_STATUS_STOPPED,		//任务停止状态
	CT_STATUS_PAUSED,		//任务暂停解密状态
	CT_STATUS_DELETED,		//将任务设置为删除状态
	CT_STATUS_MAX
	
};
*/

void CCrackTask::resetProgress(){

	m_finish_num = 0;

	m_bsuccess = false;

	m_progress = 0.0;

	memset(m_result,0,32);
	m_runing_num = 0;

}

void CCrackTask::InitAvailableBlock()
{
	cur_crack_block = m_crackblock_map.begin();
}

//任务更新到 Running 状态
//startTask : ready--->running , 
//			  stopped---> running,
//			  pause--->running,
int CCrackTask::updateStatusToRunning(){
	
	int ret = 0;
	unsigned char tempStatus = m_status;
	CB_MAP::iterator iter_block;
	CCrackBlock *pCb = NULL;
	CCrackHash *pCCH = NULL;
//	time_t time_last;  
 //   time_last = time(NULL);  
	int tag = 0;

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Running\n",m_status);
		return ERR_CONVERTRUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;
	this->m_running_time = 0;
	//m_start_time = time_last;
	

	//设置每个HASH为运行状态
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];

		if (pCCH->m_status == HASH_STATUS_READY){

			pCCH->m_status = HASH_STATUS_RUNNING;
		}
	}

	//设置子任务为准备运行状态
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		if (pCb->m_status == WI_STATUS_WAITING){

			pCb->m_status = WI_STATUS_READY;
			if (tag == 0 ){
				this->cur_crack_block = iter_block;
				tag = 1;
			}
			
		}

	}

	if (tag == 0 )
		cur_crack_block = m_crackblock_map.begin();

	return ret;
}

//停止任务执行
//running -----> ready
//pause -----> ready
int CCrackTask::updateStatusToStop(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = NULL;
	CCrackBlock *pCb = NULL;
	CB_MAP::iterator iter_block;
	unsigned char john[196];


	if ((m_status != CT_STATUS_RUNNING) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Ready\n",m_status);
		return ERR_CONVERTSTOP;

	}
	
	
	//计算运行时间
	if (m_status == CT_STATUS_RUNNING)
		this->m_running_time = 0;


	m_status = CT_STATUS_READY;

	//为了通知计算节点设置
	//设置每个HASH为准备运行状态
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];
		memset(john,0,sizeof(john));
		memcpy(john,pCCH->m_john,sizeof(john));

		pCCH->Init(john);
	
		pCCH->m_status = HASH_STATUS_READY;

	}

	//设置子任务为准备运行状态
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		pCb->m_status = WI_STATUS_WAITING;

	}

	resetProgress();
	return ret;
}

//删除任务
//Ready ----> Delete 
//finishe ----> delete
//failure ----> delete 
//CT_STATUS_FINISHED,		//任务完成状态	
//	CT_STATUS_FAILURE,		//任务解密失败状态

int CCrackTask::updateStatusToDel(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = NULL;
	unsigned char john[196];
	CCrackBlock *pCb = NULL;

	if ((m_status != CT_STATUS_READY) &&
			(m_status != CT_STATUS_FINISHED) && (m_status != CT_STATUS_FAILURE)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Delete\n",m_status);
		return ERR_CONVERTDELETE;

	}
	
	m_status = CT_STATUS_DELETED;


	//为了通知计算节点设置
	//设置每个HASH为准备运行状态
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];
		memset(john,0,sizeof(john));
		memcpy(john,pCCH->m_john,sizeof(john));

		pCCH->Init(john);
	
		pCCH->m_status = HASH_STATUS_READY;
	}

	//设置子任务为准备运行状态
	CB_MAP::iterator iter_block;
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		pCb->m_status = WI_STATUS_WAITING;
	}

	return ret;
}

//执行暂停任务
//running ---> pause
int CCrackTask::updateStatusToPause(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = NULL;
	unsigned char john[196];
	CCrackBlock *pCb = NULL;
	time_t mytime = time(NULL);

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Pause\n",m_status);
		return ERR_CONVERTPAUSE;

	}

	//计算运行时间
	this->m_running_time += (mytime-this->m_start_time);

	
	m_status = CT_STATUS_PAUSED;

	//为了通知计算节点设置
	//设置每个HASH为准备运行状态
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];
		memset(john,0,sizeof(john));
		memcpy(john,pCCH->m_john,sizeof(john));

		pCCH->Init(john);
	
		if (pCCH->m_status == HASH_STATUS_RUNNING){

			pCCH->m_status = HASH_STATUS_READY;
		}

	}

	//设置子任务为准备运行状态
	CB_MAP::iterator iter_block;
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		if ((pCb->m_status == WI_STATUS_RUNNING)||(pCb->m_status == WI_STATUS_READY)){
	

			pCb->m_status = WI_STATUS_WAITING;
		}

	}
	return ret;


}

//设置任务结束
//crackblock ---> find pass, set finish
//         ---> m_progress = 100.0 , set finish
//		   running ----> finish


int CCrackTask::updateStatusToFinish(struct crack_result *result,int hash_index){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = m_crackhash_list[hash_index];
	time_t mytime = time(NULL);

	switch(result->status){
		
		case WI_STATUS_CRACKED:
			
			pCCH->m_progress = 100.0;
			memcpy(pCCH->m_result,result->password,sizeof(result->password));
			pCCH->m_status = HASH_STATUS_FINISH;
			setCrackBlockStatus(WI_STATUS_NOT_NEED,hash_index);
			

			//可能这里会多算了一个
			this->m_finish_num +=1;

			//判断任务Task 是否结束
			ret = setFinishByHash();
			if (ret == 1){

				this->m_bsuccess = true;
				this->m_progress =100.0;
				this->m_status = CT_STATUS_FINISHED;
				//计算运行时间
				this->m_running_time +=(mytime-this->m_start_time);
				this->m_remain_time = 0;
			}

		
			break;
		case WI_STATUS_NO_PWD:
			
			//当前hash 是否解密完成，未解密完成，继续解密;解密完成，设置状态
			
			this->m_finish_num +=1;	

			//ret =1 , hash 中的所有block 都已结束; ret=0, hash 中包含未计算完成的block 
			ret = checkBlockOfHash(hash_index);
			if (ret == 1){
				
				pCCH->m_status = HASH_STATUS_NO_PASS;
				pCCH->m_progress = 100.0;

				//ret = 1, 任务中所有的hash 都已计算完成;ret = 0, 任务中的hash 并没有全部计算完成
				ret = setFinishByHash();
				if (ret == 1){

					this->m_status = CT_STATUS_FINISHED;
					this->m_progress = 100.0;
					//计算运行时间
					this->m_running_time +=(mytime-this->m_start_time);
					this->m_remain_time = 0;

				}
			}
			break;
		default :

			ret = -1;
			break;

	}

	return ret;
}

//设置任务执行失败
//crackblock ---> failure , set failure
//			running ---> failure
int CCrackTask::updateStatusToFail(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	time_t mytime = time(NULL);

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"updateStatusToFail: Task %s Status %d，Can't Set to Failure\n", guid, m_status);
		return ERR_CONVERTFAIL;

	}
	//......
	this->m_running_time +=(mytime-this->m_start_time);
	this->m_remain_time = 0;

	m_status = CT_STATUS_FAILURE;
	return ret;
}


int CCrackTask::setCrackBlockStatus(char status,int hash_index){

	int ret =0 ;
	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;

	for (iter_block = m_crackblock_map.begin() ; iter_block != m_crackblock_map.end(); iter_block++){

		pCB = iter_block->second;

		if (pCB->hash_idx == hash_index){
			
			if (pCB->m_status == WI_STATUS_READY){

				pCB->m_status = status;
				pCB->m_starttime = pCB->m_finishtime = time(NULL);
				this->m_runing_num +=1;
				m_finish_num +=1;

				//持久化
				g_Persistence.UpdateOneBlock(pCB);


			}else if ((pCB->m_status == WI_STATUS_RUNNING) ||(pCB->m_status == WI_STATUS_LOCK)){

				pCB->m_status = status;
				pCB->m_finishtime = time(NULL);
				m_finish_num +=1;

				//持久化
				g_Persistence.UpdateOneBlock(pCB);


				//添加通过心跳通知机制
				g_CrackBroker.setCompBlockStatus(pCB->m_comp_guid, pCB->guid, STATUS_NOTICE_STOP);
				
			}


		}
	}
	return ret;

}

int CCrackTask::setFinishByHash(){

	int i = 0;
	int ret = 1;

	for(i = 0;i < m_crackhash_list.size();i ++ ){
		
		if ((m_crackhash_list[i]->m_status == HASH_STATUS_READY) || (m_crackhash_list[i]->m_status == HASH_STATUS_RUNNING)){
			ret = 0;
			break;
		}
	}

	if (ret){
		
		this->m_status = CT_STATUS_FINISHED;
		this->m_progress = 100.0;
	}

	return ret; //如果ret = 1 , 任务所有的hash 都解密完成;ret=0, 包含未解密完成的hash
}


int CCrackTask::checkBlockOfHash(int hash_index){

	int ret =1 ;
	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;

	for (iter_block = m_crackblock_map.begin() ; iter_block != m_crackblock_map.end(); iter_block++){

		pCB = iter_block->second;

		if((pCB->hash_idx == hash_index) && ((pCB->m_status == WI_STATUS_READY) || (pCB->m_status ==WI_STATUS_RUNNING) || (pCB->m_status ==WI_STATUS_LOCK))){

			ret = 0;
			break;
		}
	}
	return ret;
}



void CCrackTask::RefreshRemainTime(){


	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;
	unsigned int max_remain_time = 0;

	for (iter_block = m_crackblock_map.begin() ; iter_block != m_crackblock_map.end(); iter_block++){

		pCB = iter_block->second;

		
		if (max_remain_time < pCB->m_remaintime){

			max_remain_time = pCB->m_remaintime;

			//CLog::Log(LOG_LEVEL_WARNING,"CrackBlock GUID %s,Remain time : %d\n",pCB->guid,pCB->m_remaintime);

		}



		/*
		if((pCB->hash_idx == hash_index) && ((pCB->m_status == WI_STATUS_READY) || (pCB->m_status ==WI_STATUS_RUNNING))){

			ret = 0;
			break;
		}
		*/
	}

	this->m_remain_time = max_remain_time;


}

void CCrackTask::startTime(){

	time_t time_last;
	

	if (this->m_runing_num == 1){
		
		time_last = time(NULL);
		this->m_start_time = time_last;

	}

}

void * CCrackTask::Alloc(int size){
		
	void *p = NULL;

	p = malloc(size);
	return p;
}

void CCrackTask::Free(void *p){

	free(p);
}

