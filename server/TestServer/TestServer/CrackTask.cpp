
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
	//ֱ�Ӱ��սṹ�帳ֵ����һ���ڴ漴�ɣ�����һ�������θ�ֵ
	crack_task* parent = this;
	memcpy(parent, pCrackTask, sizeof(crack_task));

	m_status = CT_STATUS_READY;   //״̬׼������
	
	resetProgress();  //������Ϣ����Ϊ��ʼ״̬
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

	//�����ļ��ָ��
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

	//��ʼ����ع�����
	pCrackBlock = split.split_easy(this,splitnum);
	if (!pCrackBlock){
		CLog::Log(LOG_LEVEL_WARNING,"split default Error\n");
		release_hashes_from_load(this);
		//Free(p);
		return SPLIT_HASH_ERR;
	}
	
	//����crack_block ���� CCrackBlock ����
	pCb = new CCrackBlock[splitnum];
	for (int i = 0 ; i < splitnum;i ++ ){
		
		pCb[i].Init(&pCrackBlock[i]);
		pCb[i].task = this;
		
		m_crackblock_map.insert(CB_MAP::value_type(pCb[i].guid,&pCb[i]));
		if(this->special == 0)
			CLog::Log(LOG_LEVEL_WARNING,"Crack Block is %s,%s %d,%d\n",pCb[i].john, pCb[i].guid ,pCb[i].start,pCb[i].end);
	}

	//�ͷ���Դ
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
	WI_STATUS_WAITING = 1,	//����ù�����󣬹�����ĳ�ʼ״̬ 
	WI_STATUS_READY,		//��������
	*/
		}else if ((pCB->m_status == WI_STATUS_WAITING) || (pCB->m_status == WI_STATUS_READY)){
			
			fprogress +=0;

		}else {

			fprogress +=100;  //������������״̬�Ľ��Ȱ���100������
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

	CT_STATUS_READY = 1,   //����ľ���״̬
	CT_STATUS_FINISHED,		//�������״̬	
	CT_STATUS_FAILURE,		//�������ʧ��״̬
	CT_STATUS_RUNNING,		//�������ڽ���״̬

	CT_STATUS_STOPPED,		//����ֹͣ״̬
	CT_STATUS_PAUSED,		//������ͣ����״̬
	CT_STATUS_DELETED,		//����������Ϊɾ��״̬
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

//������µ� Running ״̬
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
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Running\n",m_status);
		return NOT_CONVERT_TO_RUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;
	m_start_time = time_last;
	

	//����ÿ��HASHΪ����״̬
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];

		if (pCCH->m_status == HASH_STATUS_READY){

			pCCH->m_status = HASH_STATUS_RUNNING;
		}
	}

	//����������Ϊ׼������״̬
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

//ֹͣ����ִ��
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
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Ready\n",m_status);
		return NOT_CONVERT_TO_READY;

	}
	
	
	//��������ʱ��
	if (m_status == CT_STATUS_RUNNING)
		this->m_running_time = 0;


	m_status = CT_STATUS_READY;

	//Ϊ��֪ͨ����ڵ�����
	//����ÿ��HASHΪ׼������״̬
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];
		memset(john,0,sizeof(john));
		memcpy(john,pCCH->m_john,sizeof(john));

		pCCH->Init(john);
	
		pCCH->m_status = HASH_STATUS_READY;

	}

	//����������Ϊ׼������״̬
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		pCb->m_status = WI_STATUS_WAITING;

	}

	resetProgress();
	return ret;
}

//ɾ������
//Ready ----> Delete 
//finishe ----> delete
//failure ----> delete 
//CT_STATUS_FINISHED,		//�������״̬	
//	CT_STATUS_FAILURE,		//�������ʧ��״̬

int CCrackTask::updateStatusToDel(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = NULL;
	unsigned char john[196];
	CCrackBlock *pCb = NULL;

	if ((m_status != CT_STATUS_READY) &&
			(m_status != CT_STATUS_FINISHED) && (m_status != CT_STATUS_FAILURE)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Delete\n",m_status);
		return NOT_CONVERT_TO_DELETE;

	}
	
	m_status = CT_STATUS_DELETED;


	//Ϊ��֪ͨ����ڵ�����
	//����ÿ��HASHΪ׼������״̬
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];
		memset(john,0,sizeof(john));
		memcpy(john,pCCH->m_john,sizeof(john));

		pCCH->Init(john);
	
		pCCH->m_status = HASH_STATUS_READY;
	}

	//����������Ϊ׼������״̬
	CB_MAP::iterator iter_block;
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		pCb->m_status = WI_STATUS_WAITING;
	}

	return ret;
}

//ִ����ͣ����
//running ---> pause
int CCrackTask::updateStatusToPause(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = NULL;
	unsigned char john[196];
	CCrackBlock *pCb = NULL;
	time_t mytime = time(NULL);

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Pause\n",m_status);
		return NOT_CONVERT_TO_PAUSED;

	}

	//��������ʱ��
	this->m_running_time += (mytime-this->m_start_time);

	
	m_status = CT_STATUS_PAUSED;

	//Ϊ��֪ͨ����ڵ�����
	//����ÿ��HASHΪ׼������״̬
	for(int i = 0;i < count ;i ++ ){
		
		pCCH = this->m_crackhash_list[i];
		memset(john,0,sizeof(john));
		memcpy(john,pCCH->m_john,sizeof(john));

		pCCH->Init(john);
	
		if (pCCH->m_status == HASH_STATUS_RUNNING){

			pCCH->m_status = HASH_STATUS_READY;
		}

	}

	//����������Ϊ׼������״̬
		CB_MAP::iterator iter_block;
	for(iter_block = m_crackblock_map.begin();iter_block != m_crackblock_map.end();iter_block++){
	
		pCb = iter_block->second;
		if ((pCb->m_status == WI_STATUS_RUNNING)||(pCb->m_status == WI_STATUS_READY)){
	

			pCb->m_status = WI_STATUS_WAITING;
		}

	}
	return ret;


}

//�����������
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

			//�ж�����Task �Ƿ����
			ret = setFinishByHash();
			if (ret == 1){

				this->m_bsuccess = true;
				this->m_progress =100.0;
				this->m_status = CT_STATUS_FINISHED;
				//��������ʱ��
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

				//����block Ϊ����Ҫ����

				setCrackBlockStatus(WI_STATUS_NOT_NEED);

			}
			
			ret = 1;
			*/

			break;
		case WORK_ITEM_UNCRACKED:
			
			//��ǰhash �Ƿ������ɣ�δ������ɣ���������;������ɣ�����״̬
			
			this->m_finish_num +=1;

			ret = checkBlockOfHash(hash_index);
			if (ret == 1){
				
				pCCH->m_status = HASH_STATUS_NO_PASS;
				pCCH->m_progress = 100.0;

				ret = setFinishByHash();
				if (ret == 1){

					this->m_status = CT_STATUS_FINISHED;
					this->m_progress = 100.0;
					//��������ʱ��
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

//��������ִ��ʧ��
//crackblock ---> failure , set failure
//			running ---> failure
int CCrackTask::updateStatusToFail(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	time_t mytime = time(NULL);

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Failure\n",m_status);
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

	return ret; //���ret = 1 , �������е�hash ���������;ret=0, ����δ������ɵ�hash
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

