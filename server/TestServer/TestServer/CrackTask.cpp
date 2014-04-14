
#include <string.h>
#include <stdlib.h>

#include "CrackTask.h"
#include "loadfiles.h"
#include "CLog.h"
#include "split.h"
#include "macros.h"
#include "CrackHash.h"

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

	m_start_time = 0;

	m_running_time = 0;
	m_remain_time = 0;

	return 0;
}


int CCrackTask::SplitTaskFile(char *pguid){

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

	memcpy(guid,pguid,40);

	count = 0;

	//调用文件分割函数
	memset(filename,0,sizeof(filename));
	sprintf((char *)filename,".\\tempdir\\%s",guid);
	
	ret = load_hashes_file2((char *)filename,this);
	if (ret <= 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"load the hash Info Error\n");
	//	Free(p);
		return LOAD_FILE_ERR;
	}
	
	pCCH = new CCrackHash[count];
	for (i = 0 ;i < ret ;i ++ ){

		pCCH[i].Init((unsigned char *)this->hashes[i].hash);

		m_crackhash_list.push_back(&pCCH[i]);

		if(this->special == 0)
			CLog::Log(LOG_LEVEL_WARNING,"Crack Hash is %s,%s,%s\n",hashes[i].hash,hashes[i].salt,hashes[i].salt2);
	}

	//出始化相关工作项
	pCrackBlock = split.split_easy(this,splitnum);
	if (!pCrackBlock){
		CLog::Log(LOG_LEVEL_WARNING,"split default Error\n");
		release_hashes_from_load(this);
		//Free(p);
		return SPLIT_HASH_ERR;
	}
	
	//根据crack_block 创建 CCrackBlock 对象
	pCb = new CCrackBlock[splitnum];
	for (int i = 0 ; i < splitnum;i ++ ){
		
		pCb[i].Init(&pCrackBlock[i]);
		pCb[i].task = this;
		
		m_crackblock_map.insert(CB_MAP::value_type(pCb[i].guid,&pCb[i]));
		if(this->special == 0)
			CLog::Log(LOG_LEVEL_WARNING,"Crack Block is %s,%s %d,%d\n",pCb[i].john, pCb[i].guid ,pCb[i].start,pCb[i].end);
	}

	//释放资源
	release_hashes_from_load(this);
	this->hashes = NULL;
	split.release_splits((char *)pCrackBlock);
	//Free(p);
	this->m_split_num = splitnum;
	this->m_finish_num = 0;
	count = ret;
	ret = 0;
	return ret;
}


CCrackBlock *CCrackTask::GetAReadyWorkItem(){
	
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;
	CB_MAP::iterator iter_block_end = m_crackblock_map.end();
	CB_MAP::iterator iter_block_begin = m_crackblock_map.begin();

	iter_block = cur_crack_block;
	do{
		if (iter_block->second->m_status == WI_STATUS_READY){

			pCB = iter_block->second;
			pCB->m_status = WI_STATUS_RUNNING;
			m_runing_num ++;
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

	for (iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){

		pCB = iter_block->second;
		if (pCB->m_status == WI_STATUS_RUNNING){
			
			fprogress += pCB->m_progress;
	/*
	WI_STATUS_WAITING = 1,	//分配好工作项后，工作项的初始状态 
	WI_STATUS_READY,		//任务启动
	*/
		}else if ((pCB->m_status == WI_STATUS_WAITING) || (pCB->m_status == WI_STATUS_READY)){
			
			fprogress +=0;

		}else {

			fprogress +=100;  //设置了其他的状态的进度按照100来计算
		}

	}

	int blocknum = m_crackblock_map.size();
	this->m_progress = fprogress/blocknum;	

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
			ret = NOT_SUPPORT_CT_STATUS;
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
	time_t time_last;  
    time_last = time(NULL);  
	int tag = 0;

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Running\n",m_status);
		return NOT_CONVERT_TO_RUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;
	m_start_time = time_last;
	

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
		return NOT_CONVERT_TO_READY;

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
		return NOT_CONVERT_TO_DELETE;

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
		return NOT_CONVERT_TO_PAUSED;

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
		
		case WORK_ITEM_CRACKED:
			
			pCCH->m_progress = 100.0;
			memcpy(pCCH->m_result,result->password,sizeof(result->password));
			pCCH->m_status = HASH_STATUS_FINISH;
			setCrackBlockStatus(WI_STATUS_NOT_NEED,hash_index);
			

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

		/*	m_status = CT_STATUS_FINISHED;
			this->m_bsuccess = true;
			this->m_finish_num +=1;
			memcpy(this->m_result,result->password,32);
			if (this->m_finish_num == this->m_split_num ){
				
				this->m_progress = 100.0;
			}else{

				//设置block 为不需要运行

				setCrackBlockStatus(WI_STATUS_NOT_NEED);

			}
			
			ret = 1;
			*/

			break;
		case WORK_ITEM_UNCRACKED:
			
			//当前hash 是否解密完成，未解密完成，继续解密;解密完成，设置状态
			
			this->m_finish_num +=1;

			ret = checkBlockOfHash(hash_index);
			if (ret == 1){
				
				pCCH->m_status = HASH_STATUS_NO_PASS;
				pCCH->m_progress = 100.0;

				ret = setFinishByHash();
				if (ret == 1){

					this->m_status = CT_STATUS_FINISHED;
					this->m_progress = 100.0;
					//计算运行时间
					this->m_running_time +=(mytime-this->m_start_time);
					this->m_remain_time = 0;

				}
			}
				

		/*	this->m_finish_num +=1;
			if (this->m_status != CT_STATUS_FINISHED){
								
				if (this->m_finish_num == this->m_split_num){

					this->m_bsuccess = false;
					this->m_progress = 100.0;
					this->m_status = CT_STATUS_FINISHED;
					ret = 1;
				}
					
			}
			*/

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
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Failure\n",m_status);
		return NOT_CONVERT_TO_FAIL;

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

		if((pCB->hash_idx == hash_index) && ((pCB->m_status == WI_STATUS_READY) || (pCB->m_status ==WI_STATUS_RUNNING))){

			pCB->m_status = status;
			this->m_runing_num +=1;

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

		if((pCB->hash_idx == hash_index) && ((pCB->m_status == WI_STATUS_READY) || (pCB->m_status ==WI_STATUS_RUNNING))){

			ret = 0;
			break;
		}
	}
	return ret;
}

void * CCrackTask::Alloc(int size){
		
	void *p = NULL;

	p = malloc(size);
	return p;
}

void CCrackTask::Free(void *p){

	free(p);
}

