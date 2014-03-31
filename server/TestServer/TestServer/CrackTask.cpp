
#include <string.h>
#include <stdlib.h>

#include "CrackTask.h"
#include "loadfiles.h"
#include "CLog.h"
#include "split.h"
#include "crack_status.h"

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
	CCrackBlock *pCb = NULL;
	csplit split;
	crack_block *pCrackBlock = NULL;
	unsigned int splitnum = 0;
	void *p = NULL;
	int ret = 0;
	int mcount = 0;

	algo = pCrackTask->algo;
	charset = pCrackTask->charset;
	type = pCrackTask->type;
	special = pCrackTask->special;
	startLength = pCrackTask->startLength;
	endLength = pCrackTask->endLength;
	
	memset(filename,0,256);
	memset(guid,0,40);

	memcpy(filename,pCrackTask->filename,256);
	memcpy(guid,pCrackTask->guid,40);

	count = 0;

	//calculate the crack_hash 
	if (special){
		mcount = HASH_NUM_IN_TASK;
		p = Alloc(CH_LEN*HASH_NUM_IN_TASK);
	}else{
		mcount = 1;
		p = Alloc(CH_LEN);
	}
	
	if (!p) {
		CLog::Log(LOG_LEVEL_WARNING,"Init Crack Task Error\n");
		return -1;
	}
	//�����ļ��ָ��
	if (special){
		ret = load_hashes_file((char *)filename,algo,(struct crack_hash*)p,mcount);
	}else{
		ret = load_single_hash((char *)filename, algo, (struct crack_hash *)p);
	}
	
	if (ret < 0 ){
		CLog::Log(LOG_LEVEL_WARNING,"load the hash Info Error\n");
		Free(p);
		return -2;
	}
	
	count = ret;
	hashes = (struct crack_hash *) p;

	//��ʼ����ع�����
	pCrackBlock = split.split_default(this,splitnum);
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
		
	}
	//�ͷ���Դ
	split.release_splits((char *)pCrackBlock);
	Free(p);
	
	m_status = CT_STATUS_READY;   //״̬׼������
	
	m_split_num = splitnum;
	resetProgress();  //������Ϣ����Ϊ��ʼ״̬
	memset(m_control_guid,0,40);

	m_priority = CT_PRIORITY_NORMAL;

	return 0;
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

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Running\n",m_status);
		return NOT_CONVERT_TO_RUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;
	return ret;
}

//ֹͣ����ִ��
//running -----> ready
//pause -----> ready
int CCrackTask::updateStatusToStop(){

	int ret = 0;
	unsigned char tempStatus = m_status;

	if ((m_status != CT_STATUS_RUNNING) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Ready\n",m_status);
		return NOT_CONVERT_TO_READY;

	}
	
	m_status = CT_STATUS_READY;
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

	if ((m_status != CT_STATUS_READY) &&
			(m_status != CT_STATUS_FINISHED) && (m_status != CT_STATUS_FAILURE)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Delete\n",m_status);
		return NOT_CONVERT_TO_DELETE;

	}
	
	m_status = CT_STATUS_DELETED;
	return ret;



}

//ִ����ͣ����
//running ---> pause
int CCrackTask::updateStatusToPause(){

	int ret = 0;
	unsigned char tempStatus = m_status;

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Pause\n",m_status);
		return NOT_CONVERT_TO_PAUSED;

	}
	
	m_status = CT_STATUS_PAUSED;
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

int CCrackTask::updateStatusToFinish(struct crack_result *result){

	int ret = 0;
	unsigned char tempStatus = m_status;

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
			
			m_status = CT_STATUS_FINISHED;
			this->m_bsuccess = true;
			this->m_finish_num +=1;
			memcpy(this->m_result,result->password,32);
			if (this->m_finish_num == this->m_split_num ){
				
				this->m_progress = 100.0;
			}else{

				//���õ�ǰ���е�block Ϊ����Ҫ����
				setCrackBlockStatus(WI_STATUS_NOT_NEED);

			}
			
			ret = 1;
			break;
		case WORK_ITEM_UNCRACKED:
			
			this->m_finish_num +=1;
			if (this->m_status != CT_STATUS_FINISHED){
								
				if (this->m_finish_num == this->m_split_num){

					this->m_bsuccess = false;
					this->m_progress = 100.0;
					this->m_status = CT_STATUS_FINISHED;
					ret = 1;
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

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d��Can't Set to Pause\n",m_status);
		return NOT_CONVERT_TO_PAUSED;

	}
	//......



	m_status = CT_STATUS_FAILURE;
	return ret;

}



int CCrackTask::setCrackBlockStatus(char status){

	int ret =0 ;
	CB_MAP::iterator iter_block;
	CCrackBlock *pCB = NULL;

	
	for (iter_block = m_crackblock_map.begin() ; iter_block != m_crackblock_map.end(); iter_block++){

		pCB = iter_block->second;

		if((pCB->m_status == WI_STATUS_READY) || (pCB->m_status ==WI_STATUS_RUNNING)){

			
			pCB->m_status = WI_STATUS_NOT_NEED;

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

