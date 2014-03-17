#include "launcher.h"
#include "resourceslotpool.h"
#include "CLog.h"
#include "algorithm_types.h"
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
}

clauncher::~clauncher()
{
	if(m_bThreadRunning)
	{
		Stop();
	}
}

#include "HashKill.h"
Crack* hashkill = new HashKill();

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
		{
			CLog::Log(LOG_LEVEL_WARNING,"clauncher: found non resource\n");
			goto next;
		}
		
		//处理
		switch(uStatus)
		{
			case RS_STATUS_AVAILABLE:
				{
					//提交给解密插件执行，执行完毕设置执行结果
					CLog::Log(LOG_LEVEL_NOMAL,"clauncher: doing crack task\n");
					ResourcePool::Get().SetToOccupied(prsp);

					//调用解密插件执行解密任务
					crack_block* block = new crack_block();
					block->algo = algo_md5;
					block->charset = charset_lalphanum;
					block->type = bruteforce;
					sprintf(block->guid, "%p", block);
					strcpy(block->john, "63cb5261f4610ba648fcd5e1b72c3173");
					block->start = 4;
					block->end = 7;
					hashkill->StartCrack(block, block->guid, prsp->m_worker_type == DEVICE_GPU, prsp->m_device);
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
	
	pthread_mutex_t running_mutex;
	pthread_cond_t keeprunning_cv;
	pthread_mutex_init(&running_mutex, NULL);
	pthread_cond_init(&keeprunning_cv, NULL);
	
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
