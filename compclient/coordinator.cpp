#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"pthreadVC2.lib")
#else
#include <unistd.h>
#include <errno.h>
#define Sleep(a) usleep(a*1000)
#endif
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
	
	while(1)
	{
		if(p->m_bStop) break;
			
		//从资源池获取可用的计算单元
		ResourcePool::Get().Lock();
		prsp = ResourcePool::Get().CoordinatorQuery(status);

		if(!status)
			goto next;
		
		CLog::Log(LOG_LEVEL_NOMAL, "ccoordinator: fetch workitem %d\n", item.algo);
		
		//处理
		if(status == RS_STATUS_READY)
		{
			//这里需要从网络获取workitem数据，只有有数据才会进行下一步
			ret = Client::Get().GetWorkItemFromServer(&item, sizeof(item));
			if(ret < 0)	continue;
		
			//从服务器申请任务，并且将资源状态设置为RS_STATUS_AVAILABLE
			CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: allocate compute unit\n");
			ResourcePool::Get().SetToAvailable(prsp, &item);
		}
		else if(status == RS_STATUS_RECOVERED)
		{	
			//提交结果到服务器，并释放资源池
			CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: submit result\n");
			ResourcePool::Get().SetToReady(prsp);
		}
		else if(status == RS_STATUS_UNRECOVERED)
		{	
			//提交结果到服务器，并释放资源池
			CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: submit result\n");
			ResourcePool::Get().SetToReady(prsp);
		}
		
next:	
		ResourcePool::Get().UnLock();
		Sleep(3000);
	}
	
	fprintf(stderr, "leaving %s\n", __FUNCTION__);
	return 0;
}
void ccoordinator::Start(void)//开始扫描线程
{	
	if(m_bThreadRunning)
	{
		CLog::Log(LOG_LEVEL_WARNING,"ccoordinator 扫描线程运行中，不需要再创建\n");
		return;
	}
	
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
	{
		CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator 没有扫描线程在运行\n");
		return;
	}

	m_bStop = 1;
	int returnValue = pthread_join(m_pThread, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"ccoordinator 线程退出失败，错误代码: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator 扫描线程成功退出\n");
	}
	m_bThreadRunning = 0;
}
