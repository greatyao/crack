
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

/*
unsigned char algo;		//解密算法
	unsigned char charset;	//解密字符集
	unsigned char type;		//解密类型
	unsigned char special;	//是否是文件解密（pdf+office+rar+zip）
	unsigned char startLength;//起始长度
	unsigned char endLength;	//终结长度
	unsigned char filename[256];	//用户传过来的文件名
	char guid[40];			//用户端的任务的GUID
	int count;				//需要解密的Hash个数（如果是文件=1）
	struct crack_hash* hashes;			//这里需要动态申请
	*/
int CCrackTask::Init(crack_task *pCrackTask)
{
	algo = pCrackTask->algo;
	charset = pCrackTask->charset;
	type = pCrackTask->type;
	special = pCrackTask->special;
	startLength = pCrackTask->startLength;
	endLength = pCrackTask->endLength;
	
	memset(filename,0,256);
	memset(guid,0,40);
	memcpy(guid,pCrackTask->guid,40);

	m_status = CT_STATUS_READY;   //状态准备运行
	
	resetProgress();  //进度信息设置为初始状态
	memset(m_control_guid,0,40);

	m_priority = CT_PRIORITY_NORMAL;

	m_file = NULL;

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

	//calculate the crack_hash 
/*	mcount = HASH_NUM_IN_TASK;
	p = Alloc(CH_LEN * HASH_NUM_IN_TASK);
	

	if (!p) {
		CLog::Log(LOG_LEVEL_WARNING,"Init Crack Task Error\n");
		return -1;
	}
*/
	//调用文件分割函数
	memset(filename,0,sizeof(filename));
	sprintf((char *)filename,".\\tempdir\\%s",guid);
	
	//ret = load_hashes_file((char *)filename,algo,(struct crack_hash*)p,mcount);
	ret = load_hashes_file2((char *)filename,this);
	if (ret < 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"load the hash Info Error\n");
		Free(p);
		return -2;
	}
	
	

//	pch =(struct crack_hash *)p;
	
	pCCH = new CCrackHash[count];
	for (i = 0 ;i < ret ;i ++ ){

		pCCH[i].Init((unsigned char *)this->hashes[i].hash);
		CLog::Log(LOG_LEVEL_WARNING,"Crack Hash is %s,%s,%s\n",hashes[i].hash,hashes[i].salt,hashes[i].salt2);
	}
	
	count = ret;
	//出始化相关工作项
	pCrackBlock = split.split_easy(this,splitnum);
	if (!pCrackBlock){
		CLog::Log(LOG_LEVEL_WARNING,"split default Error\n");
		Free(p);
		return -3;
	}
	
	//根据crack_block 创建 CCrackBlock 对象
	pCb = new CCrackBlock[splitnum];
	for (int i = 0 ; i < splitnum;i ++ ){
		
		pCb[i].Init(&pCrackBlock[i]);
		pCb[i].task = this;
		m_crackblock_map.insert(CB_MAP::value_type(pCb[i].guid,&pCb[i]));
		CLog::Log(LOG_LEVEL_WARNING,"Crack Block is %s,%s,%d,%d,%d,%d\n",pCb[i].john,pCb[i].guid,pCb[i].start,pCb[i].end,pCb[i].start2,pCb[i].end2);
		
	}
	
	

	//释放资源
	release_hashes_from_load(this);
	this->hashes = NULL;
	split.release_splits((char *)pCrackBlock);
	//Free(p);
	this->m_split_num = splitnum;
	this->m_finish_num = 0;

	return ret;

}


CCrackBlock *CCrackTask::GetAReadyWorkItem(){
	
	CCrackBlock *pCB = NULL;
	CB_MAP::iterator iter_block;

	for (iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){

		pCB = iter_block->second;
		if (pCB->m_status == WI_STATUS_READY){
			
		
			break;
		}

	}
	
	if (iter_block != m_crackblock_map.end()){
		
		pCB->m_status = WI_STATUS_RUNNING;
		m_runing_num ++;
			
	}else{
		
		pCB= NULL;
	}
	
	
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

			fprogress +=1;
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

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Running\n",m_status);
		return NOT_CONVERT_TO_RUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;

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
		}

	}

	

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


	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Pause\n",m_status);
		return NOT_CONVERT_TO_PAUSED;

	}
	
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
/*
struct crack_result
{
	char guid[40];			//workitem/crack_block的GUID
	unsigned int status;	//workitem的结果状态
	char password[32];		//如果解密成功，存放密码
};

*/

int CCrackTask::updateStatusToFinish(struct crack_result *result,int hash_index){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = m_crackhash_list[hash_index];

	//设置相关进度信息
	/*
	#define WORK_ITEM_AVAILABLE		0	//workitem的起始状态，可供其他计算单元使用
#define WORK_ITEM_LOCK			1	//workitem已经被一个计算单元占用，但是不确定计算单元的解密任务是否进行，此时不能被其他计算单元使用
#define WORK_ITEM_UNLOCK		2	//计算单元通知服务端unlock该资源，重新设置为avaiable，以供其他计算节点使用
#define WORK_ITEM_WORKING		3	//计算单元正在对该workitem进行解密任务
#define WORK_ITEM_CRACKED		4	//计算单元完成解密任务，同时破解出密码
#define WORK_ITEM_UNCRACKED		5	//计算单元完成解密任务，但没有破解出密码
	*/

	//....

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

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Failure\n",m_status);
		return NOT_CONVERT_TO_FAIL;

	}
	//......

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

