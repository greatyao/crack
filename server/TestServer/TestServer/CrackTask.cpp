
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
	//ֱ�Ӱ��սṹ�帳ֵ����һ���ڴ漴�ɣ�����һ�������θ�ֵ
	crack_task* parent = this;
	memcpy(parent, pCrackTask, sizeof(crack_task));

	m_status = CT_STATUS_READY;   //״̬׼������
	
	resetProgress();  //������Ϣ����Ϊ��ʼ״̬
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

	//�����ļ��ָ��
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

	//��ʼ����ع�����
	pCrackBlock = split.split_task(this,splitnum);
	if (!pCrackBlock){
		CLog::Log(LOG_LEVEL_WARNING,"SplitTaskFile: split_task Error\n");
		release_hashes_from_load(this);
		return ERR_SPLITTASK;
	}
	CLog::Log(LOG_LEVEL_NOMAL,"SplitTaskFile: Wonderful! We split task by %d blocks\n", splitnum);		
	
	//����crack_block ���� CCrackBlock ����
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

	//�ͷ���Դ
	release_hashes_from_load(this);
	this->hashes = NULL;
	split.release_splits((char *)pCrackBlock);
	this->m_split_num = splitnum;
	this->m_finish_num = 0;
	count = ret;
	ret = 0;
	return ret;
}

//�����ӵĻ�ȡblock �����������˶�block �е�comp_guid �ĸ�ֵ
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
			pCB->m_status = WI_STATUS_LOCK;  //���������ȱ�����ڵ�������Ȼ����ݴ���״̬����Unlock,Running
			m_runing_num ++;

			//������ڵ����Ϣ��ֵ��block,ipinfo����Ϊip:port
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

			fprogress +=100;  //������������״̬�Ľ��Ȱ���100������
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

void CCrackTask::InitAvailableBlock()
{
	cur_crack_block = m_crackblock_map.begin();
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
//	time_t time_last;  
 //   time_last = time(NULL);  
	int tag = 0;

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Running\n",m_status);
		return ERR_CONVERTRUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;
	this->m_running_time = 0;
	//m_start_time = time_last;
	

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
		return ERR_CONVERTSTOP;

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
		return ERR_CONVERTDELETE;

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
		return ERR_CONVERTPAUSE;

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
		
		case WI_STATUS_CRACKED:
			
			pCCH->m_progress = 100.0;
			memcpy(pCCH->m_result,result->password,sizeof(result->password));
			pCCH->m_status = HASH_STATUS_FINISH;
			setCrackBlockStatus(WI_STATUS_NOT_NEED,hash_index);
			

			//��������������һ��
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

		
			break;
		case WI_STATUS_NO_PWD:
			
			//��ǰhash �Ƿ������ɣ�δ������ɣ���������;������ɣ�����״̬
			
			this->m_finish_num +=1;	

			//ret =1 , hash �е�����block ���ѽ���; ret=0, hash �а���δ������ɵ�block 
			ret = checkBlockOfHash(hash_index);
			if (ret == 1){
				
				pCCH->m_status = HASH_STATUS_NO_PASS;
				pCCH->m_progress = 100.0;

				//ret = 1, ���������е�hash ���Ѽ������;ret = 0, �����е�hash ��û��ȫ���������
				ret = setFinishByHash();
				if (ret == 1){

					this->m_status = CT_STATUS_FINISHED;
					this->m_progress = 100.0;
					//��������ʱ��
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

//��������ִ��ʧ��
//crackblock ---> failure , set failure
//			running ---> failure
int CCrackTask::updateStatusToFail(){

	int ret = 0;
	unsigned char tempStatus = m_status;
	time_t mytime = time(NULL);

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"updateStatusToFail: Task %s Status %d��Can't Set to Failure\n", guid, m_status);
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

				//�־û�
				g_Persistence.UpdateOneBlock(pCB);


			}else if ((pCB->m_status == WI_STATUS_RUNNING) ||(pCB->m_status == WI_STATUS_LOCK)){

				pCB->m_status = status;
				pCB->m_finishtime = time(NULL);
				m_finish_num +=1;

				//�־û�
				g_Persistence.UpdateOneBlock(pCB);


				//���ͨ������֪ͨ����
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

	return ret; //���ret = 1 , �������е�hash ���������;ret=0, ����δ������ɵ�hash
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

