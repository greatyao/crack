﻿#include "launcher.h"
#include "resourceslotpool.h"
#include "CLog.h"
#include "algorithm_types.h"
#include "Client.h"
#include "CrackManager.h"

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
	CrackManager::Get().RegisterCallback(ReportDone, ReportStatus);
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
	//struct _resourceslotpool *prsp;
	//prsp = ResourcePool::Get().QueryByGuid(guid);
	//if(!prsp)	return -1;
	struct _resourceslotpool* rs[MAX_PARALLEL_NUM];
	int k = ResourcePool::Get().QueryByGuid(rs, MAX_PARALLEL_NUM, guid);
	if(k == 0)	return -1;
	
	ResourcePool::Get().Lock();
	
	if(cracked)
		CLog::Log(LOG_LEVEL_NOTICE, "clauncher: Crack password %s [guid=%s]\n", passwd, guid);
	else
		CLog::Log(LOG_LEVEL_ERROR, "clauncher: Crack non password [guid=%s]\n", guid);
	
	//ResourcePool::Get().SetToRecover(prsp, cracked, passwd);
	ResourcePool::Get().SetToRecover(rs, k, cracked, passwd);
	
	ResourcePool::Get().UnLock();
	
	return 0;
}

int clauncher::ReportStatus(char* guid, int progress, float speed, unsigned int remainTime)
{
	CLog::Log(LOG_LEVEL_NOMAL, "clauncher: Progress:%d%% Speed:%g c/s Remaing %u sec\n", 
		progress, speed, remainTime);
		
	crack_status status;
	strncpy(status.guid, guid, sizeof(status.guid));
	status.progress = progress;
	status.speed = speed;
	status.remainTime = remainTime;
	Client::Get().ReportStatusToServer(&status);
	
	return 0;
}

void *clauncher::Thread(void*par)//扫描线程
{
	clauncher *p = (clauncher*)par;
	unsigned uStatus = 0;
	ResourcePool& pool = ResourcePool::Get();
	struct _resourceslotpool* rs[MAX_PARALLEL_NUM];
	unsigned short deviceIds[MAX_PARALLEL_NUM];

	while(1)
	{
		if(p->m_bStop) break;
		
		//do
		pool.Lock();
		//prsp = pool.LauncherQuery(uStatus);
		//if(!uStatus)	goto next;
		
		int k = pool.LauncherQuery(rs, MAX_PARALLEL_NUM);
		if(k == 0) goto next;
		uStatus = rs[0]->m_rs_status;
		CLog::Log(LOG_LEVEL_NOMAL, "clauncher: LauncherQuery %d %s\n", k, status_msg[uStatus]);
		
		//处理
		switch(uStatus)
		{
			case RS_STATUS_AVAILABLE:
				{
					//提交给解密插件执行，执行完毕设置执行结果
					crack_block* block = rs[0]->m_item;
					bool gpu = rs[0]->m_worker_type == DEVICE_GPU;
					for(int i = 0; i < k; i++)
						deviceIds[i] = rs[i]->m_device;
					//bool lauched = CrackManager::Get().StartCrack(block, block->guid, gpu, rs[0]->m_device) == 0;
					bool lauched = CrackManager::Get().StartCrack(block, block->guid, gpu, deviceIds, k) == 0;
					
					crack_result result;
					strcpy(result.guid, rs[0]->m_guid);
					result.status = lauched ? WORK_ITEM_WORKING : WORK_ITEM_UNLOCK;
					Client::Get().ReportResultToServer(&result);
					
					if(!lauched)	pool.SetToFailed(rs, k);
					else			pool.SetToOccupied(rs, k);
				}
				break;
			case RS_STATUS_FAILED:
				{	
					//重新初始化资源池，并释放资源池
					CLog::Log(LOG_LEVEL_NOMAL,"clauncher: Find failed task [guid=%s]\n", rs[0]->m_guid);
					pool.SetToReady(rs, k);
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
		//随便等待一下，这里仅供测试，实际不需要
		pool.UnLock();
		sleep(3);
	}
	
	CLog::Log(LOG_LEVEL_NOMAL, "claucher: Exit thread\n");
	
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
		CLog::Log(LOG_LEVEL_ERROR,"clauncher: Failed to create thread[%d]\n", errno);
		m_bStop = true;
		m_bThreadRunning = false;
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"clauncher: Create thread\n");
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
		CLog::Log(LOG_LEVEL_ERROR,"clauncher: Failed to exit thread: %d\n", errno);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"clauncher: Succeed to exit thread\n");
	}
	m_bThreadRunning = false;
}
