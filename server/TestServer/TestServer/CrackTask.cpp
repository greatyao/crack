
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
	//调用文件分割函数
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

	//出始化相关工作项
	pCrackBlock = split.split_default(this,splitnum);
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
		
	}
	//释放资源
	split.release_splits((char *)pCrackBlock);
	Free(p);
	
	m_status = CT_STATUS_READY;   //状态准备运行
	
	m_split_num = splitnum;
	resetProgress();  //进度信息设置为初始状态
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

	if ((m_status != CT_STATUS_READY) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Running\n",m_status);
		return NOT_CONVERT_TO_RUNNING;

	}
	
	m_status = CT_STATUS_RUNNING;
	return ret;
}

//停止任务执行
//running -----> ready
//pause -----> ready
int CCrackTask::updateStatusToStop(){

	int ret = 0;
	unsigned char tempStatus = m_status;

	if ((m_status != CT_STATUS_RUNNING) && (m_status != CT_STATUS_PAUSED)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Ready\n",m_status);
		return NOT_CONVERT_TO_READY;

	}
	
	m_status = CT_STATUS_READY;
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

	if ((m_status != CT_STATUS_READY) &&
			(m_status != CT_STATUS_FINISHED) && (m_status != CT_STATUS_FAILURE)){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Delete\n",m_status);
		return NOT_CONVERT_TO_DELETE;

	}
	
	m_status = CT_STATUS_DELETED;
	return ret;



}

//执行暂停任务
//running ---> pause
int CCrackTask::updateStatusToPause(){

	int ret = 0;
	unsigned char tempStatus = m_status;

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Pause\n",m_status);
		return NOT_CONVERT_TO_PAUSED;

	}
	
	m_status = CT_STATUS_PAUSED;
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

int CCrackTask::updateStatusToFinish(struct crack_result *result){

	int ret = 0;
	unsigned char tempStatus = m_status;

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
			
			m_status = CT_STATUS_FINISHED;
			this->m_bsuccess = true;
			this->m_finish_num +=1;
			memcpy(this->m_result,result->password,32);
			if (this->m_finish_num == this->m_split_num ){
				
				this->m_progress = 100.0;
			}else{

				//设置当前运行的block 为不需要运行
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

//设置任务执行失败
//crackblock ---> failure , set failure
//			running ---> failure
int CCrackTask::updateStatusToFail(){

	int ret = 0;
	unsigned char tempStatus = m_status;

	if (m_status != CT_STATUS_RUNNING){
		
		CLog::Log(LOG_LEVEL_WARNING,"Task Current is Status %d，Can't Set to Pause\n",m_status);
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

