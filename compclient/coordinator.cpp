#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"pthreadVC2.lib")
#else
#include <unistd.h>
#include <errno.h>
#define Sleep(a) usleep(a*1000)
#endif
#include <string.h>
#include "coordinator.h"
#include "resourceslotpool.h"
#include "CLog.h"
#include "Client.h"
#include "algorithm_types.h"

ccoordinator::ccoordinator()
{
	m_bStop = 0;
	m_bThreadRunning = 0;
}
ccoordinator::~ccoordinator()
{
	if(m_bThreadRunning)
	{
		Stop();
	}
}

void *ccoordinator::Thread(void*par)//扫描线程 + 从socket获取item
{
	ccoordinator *p = (ccoordinator*)par;
	struct _resourceslotpool *prsp;
	unsigned status = 0;
	int ret;
	crack_block item;
	ResourcePool& pool = ResourcePool::Get();
	
	while(1)
	{
		if(p->m_bStop) break;
			
		//从资源池获取可用的计算单元
		pool.Lock();
		prsp = pool.CoordinatorQuery(status);

		if(!status)
			goto next;
		
		//处理
		if(status == RS_STATUS_READY)
		{
			//这里需要从网络获取workitem数据，只有有数据才会进行下一步
			ret = Client::Get().GetWorkItemFromServer(&item);
			if(ret != sizeof(item))	goto next;
			CLog::Log(LOG_LEVEL_NOMAL, "ccoordinator: fetch workitem %s\n", item.john);
		
			//从服务器申请任务，并且将资源状态设置为RS_STATUS_AVAILABLE
			CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: allocate compute unit\n");
			pool.SetToAvailable(prsp, &item);
		}
		else if(status == RS_STATUS_RECOVERED || status == RS_STATUS_UNRECOVERED)
		{	
			//提交结果到服务器，并释放资源池
			CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: submit result\n");
			crack_result result;
			strcpy(result.guid, prsp->m_guid);
			if(prsp->m_is_recovered){
				result.status = WORK_ITEM_CRACKED;
				strncpy(result.password, prsp->m_password, sizeof(result.password));
			} else{
				result.status = WORK_ITEM_UNCRACKED;
			}
			//TODO:需要考虑如果服务器宕机，需要将解密结果持久化:-)
			Client::Get().ReportResultToServer(&result);
			pool.SetToReady(prsp);
		}
		
next:	
		pool.UnLock();
		sleep(3);
	}
	
	CLog::Log(LOG_LEVEL_NOMAL, "ccoordinator: Exit thread\n");
	return 0;
}
void ccoordinator::Start(void)//开始扫描线程
{	
	if(m_bThreadRunning)
		return;
	
	m_bStop = 0;
	m_bThreadRunning = 0;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	int returnValue = pthread_create( &m_pThread, NULL, ccoordinator::Thread, (void *)this);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"ccoordinator: failed to create thread[%d]\n", errno);
		m_bStop = 0;
		m_bThreadRunning = 0;
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator create thread\n");
		m_bStop = 0;
		m_bThreadRunning = 1;
	}
}
void ccoordinator::Stop(void)//停止扫描线程
{
	if(m_bThreadRunning==0)
		return;

	m_bStop = 1;
	int returnValue = pthread_join(m_pThread, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"ccoordinator: Failed to exit thread: %d\n", errno);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: Succeed to exit thread\n");
	}
	m_bThreadRunning = 0;
}
