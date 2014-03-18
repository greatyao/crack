#include "launcher.h"
#include "resourceslotpool.h"
#include "CLog.h"
#include "algorithm_types.h"
#include "HashKill.h"
Crack* hashkill = new HashKill();

#include <string.h>

#if defined(__CYGWIN__) || defined(__linux__)
#include <unistd.h>
#include <errno.h>
#define Sleep(a) usleep(a*1000)
#endif

clauncher::clauncher()
{
	m_bStop = true;
	m_bThreadRunning = false;
	hashkill->RegisterCallback(ReportDone, ReportStatus);
}

clauncher::~clauncher()
{
	if(m_bThreadRunning)
	{
		Stop();
	}
}

int clauncher::ReportDone(char* guid, bool cracked, const char* passwd)
{
	struct _resourceslotpool *prsp;
	prsp = ResourcePool::Get().QueryByGuid(guid);
	if(!prsp)	return -1;
	
	ResourcePool::Get().Lock();
	
	CLog::Log(LOG_LEVEL_NOTICE, "clauncher: complete crack task\n");
	ResourcePool::Get().SetToRecover(prsp, cracked, passwd);
	
	ResourcePool::Get().UnLock();
	
	return 0;
}

int clauncher::ReportStatus(char* guid, int progress, float speed, unsigned int remainTime)
{
	CLog::Log(LOG_LEVEL_NOMAL, "clauncher: Progress:%d%% Speed:%g c/s Remaing %u sec\n", 
		progress, speed, remainTime);
	return 0;
}

void *clauncher::Thread(void*par)//扫描线程
{
	clauncher *p = (clauncher*)par;
	struct _resourceslotpool *prsp;
	unsigned uStatus = 0;

	while(p->m_bStop!=true)
	{
		//do
		ResourcePool::Get().Lock();
		prsp = ResourcePool::Get().LauncherQuery(uStatus);
		if(!uStatus)
			goto next;
		
		//处理
		switch(uStatus)
		{
			case RS_STATUS_AVAILABLE:
				{
					//提交给解密插件执行，执行完毕设置执行结果
					crack_block* block = prsp->m_item;
					CLog::Log(LOG_LEVEL_NOMAL,"clauncher: doing crack task\n");
					if(hashkill->StartCrack(block, block->guid, prsp->m_worker_type == DEVICE_GPU, prsp->m_device) < 0){
						;
					} else{
						ResourcePool::Get().SetToOccupied(prsp);
					}
				}
				break;
			case RS_STATUS_FAILED:
				{	//重新初始化资源池，并释放资源池
					CLog::Log(LOG_LEVEL_NOMAL,"clauncher: find failed task\n");
					ResourcePool::Get().SetToReady(prsp);
				}
				break;
			//case RS_STATUS_OCCUPIED://由解密插件提交解密结果以后处理
			//	{//正在执行解密任务
			//		CLog::Log(LOG_LEVEL_NOMAL,"clauncher 解密任务执行中\n");
			//		p->m_pcrsp->SetToReady(prsp);
			//	}
				break;
			default:break;
		}
next:
		ResourcePool::Get().UnLock();
		//随便等待一下，这里仅供测试，实际不需要
		Sleep(3000);
	}
	
	fprintf(stderr, "leaving %s\n", __FUNCTION__);
	
	return 0;
}

void clauncher::Start(void)//开始扫描线程
{	
	if(m_bThreadRunning==true)
	{
		return;
	}
	
	m_bStop = false;
	m_bThreadRunning = true;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	int returnValue = pthread_create( &m_pThread, &attr, Thread, (void *)this);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"clauncher: failed to create thread[%d]\n", errno);
		m_bStop = true;
		m_bThreadRunning = false;
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"clauncher: create thread\n");
		m_bStop = false;
		m_bThreadRunning = true;
	}
}
void clauncher::Stop(void)//停止扫描线程
{
	if(m_bThreadRunning==false)
	{
		return;
	}

	m_bStop = true;
	int returnValue = pthread_join(m_pThread, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"clauncher 线程退出失败，错误代码: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"clauncher 扫描线程成功退出\n");
	}
	m_bThreadRunning = false;
}
