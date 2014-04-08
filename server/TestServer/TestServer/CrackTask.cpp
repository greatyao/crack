
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
unsigned char algo;		//�����㷨
	unsigned char charset;	//�����ַ���
	unsigned char type;		//��������
	unsigned char special;	//�Ƿ����ļ����ܣ�pdf+office+rar+zip��
	unsigned char startLength;//��ʼ����
	unsigned char endLength;	//�ս᳤��
	unsigned char filename[256];	//�û����������ļ���
	char guid[40];			//�û��˵������GUID
	int count;				//��Ҫ���ܵ�Hash������������ļ�=1��
	struct crack_hash* hashes;			//������Ҫ��̬����
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

	m_status = CT_STATUS_READY;   //״̬׼������
	
	resetProgress();  //������Ϣ����Ϊ��ʼ״̬
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
	//�����ļ��ָ��
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
	//��ʼ����ع�����
	pCrackBlock = split.split_easy(this,splitnum);
	if (!pCrackBlock){
		CLog::Log(LOG_LEVEL_WARNING,"split default Error\n");
		Free(p);
		return -3;
	}
	
	//����crack_block ���� CCrackBlock ����
	pCb = new CCrackBlock[splitnum];
	for (int i = 0 ; i < splitnum;i ++ ){
		
		pCb[i].Init(&pCrackBlock[i]);
		pCb[i].task = this;
		m_crackblock_map.insert(CB_MAP::value_type(pCb[i].guid,&pCb[i]));
		CLog::Log(LOG_LEVEL_WARNING,"Crack Block is %s,%s,%d,%d,%d,%d\n",pCb[i].john,pCb[i].guid,pCb[i].start,pCb[i].end,pCb[i].start2,pCb[i].end2);
		
	}
	
	

	//�ͷ���Դ
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
	WI_STATUS_WAITING = 1,	//����ù�����󣬹�����ĳ�ʼ״̬ 
	WI_STATUS_READY,		//��������
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

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Running\n",m_status);
		return NOT_CONVERT_TO_RUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;

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
		}

	}

	

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


	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Pause\n",m_status);
		return NOT_CONVERT_TO_PAUSED;

	}
	
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
/*
struct crack_result
{
	char guid[40];			//workitem/crack_block��GUID
	unsigned int status;	//workitem�Ľ��״̬
	char password[32];		//������ܳɹ����������
};

*/

int CCrackTask::updateStatusToFinish(struct crack_result *result,int hash_index){

	int ret = 0;
	unsigned char tempStatus = m_status;
	CCrackHash *pCCH = m_crackhash_list[hash_index];

	//������ؽ�����Ϣ
	/*
	#define WORK_ITEM_AVAILABLE		0	//workitem����ʼ״̬���ɹ��������㵥Ԫʹ��
#define WORK_ITEM_LOCK			1	//workitem�Ѿ���һ�����㵥Ԫռ�ã����ǲ�ȷ�����㵥Ԫ�Ľ��������Ƿ���У���ʱ���ܱ��������㵥Ԫʹ��
#define WORK_ITEM_UNLOCK		2	//���㵥Ԫ֪ͨ�����unlock����Դ����������Ϊavaiable���Թ���������ڵ�ʹ��
#define WORK_ITEM_WORKING		3	//���㵥Ԫ���ڶԸ�workitem���н�������
#define WORK_ITEM_CRACKED		4	//���㵥Ԫ��ɽ�������ͬʱ�ƽ������
#define WORK_ITEM_UNCRACKED		5	//���㵥Ԫ��ɽ������񣬵�û���ƽ������
	*/

	//....

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

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Failure\n",m_status);
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

