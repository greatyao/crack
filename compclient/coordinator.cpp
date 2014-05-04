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
#include "macros.h"
#include "Client.h"
#include "CrackManager.h"
#include "algorithm_types.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

static map<string, crack_status> running_status;
static vector<crack_result> done_results;
static pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;

ccoordinator::ccoordinator()
{
	m_bStop = 0;
	m_bThreadRunning = 0;
	CrackManager::Get().RegisterCallback(ReportDone, ReportStatus);
}
ccoordinator::~ccoordinator()
{
	if(m_bThreadRunning)
	{
		Stop();
	}
}

int ccoordinator::ReportDone(char* guid, bool cracked, const char* passwd, bool report)
{
	struct _resourceslotpool* rs[MAX_PARALLEL_NUM];
	int k = ResourcePool::Get().QueryByGuid(rs, MAX_PARALLEL_NUM, guid);
	if(k == 0)	return -1;
	
	ResourcePool::Lock lk(ResourcePool::Get().GetMutex());
	
	if(cracked)
		CLog::Log(LOG_LEVEL_SUCCEED, "ccoordinator: Crack password %s [guid=%s]\n", passwd, guid);
	else
		CLog::Log(LOG_LEVEL_ERROR, "ccoordinator: Crack non password [guid=%s]\n", guid);
	
	//ResourcePool::Get().SetToRecover(prsp, cracked, passwd, report);
	ResourcePool::Get().SetToRecover(rs, k, cracked, passwd, report);
	
	return 0;
}

int ccoordinator::ReportStatus(char* guid, int progress, float speed, unsigned int remainTime)
{
	CLog::Log(LOG_LEVEL_NOMAL, "ccoordinator: Progress:%d%% Speed:%g c/s Remaing %u sec\n", 
		progress, speed, remainTime);
		
	crack_status status;
	strncpy(status.guid, guid, sizeof(status.guid));
	status.progress = progress;
	status.speed = speed;
	status.remainTime = remainTime;
	
	ResourcePool::Lock lk(&running_mutex);
	running_status[guid] = status;
	
	return 0;
}

void *ccoordinator::Thread(void*par)//扫描线程 + 从socket获取item
{
	ccoordinator *p = (ccoordinator*)par;
	unsigned status = 0;
	int ret;
	crack_block item;
	ResourcePool& pool = ResourcePool::Get();
	int crack_device = CrackManager::Get().UsingCPU() ? -1 : DEVICE_GPU;
	struct _resourceslotpool* rs[MAX_PARALLEL_NUM];
		
	while(1)
	{
		if(p->m_bStop) break;
			
		sleep(3);
		
		//上报任务状态和上次提交失败的已解密结束的任务
		if(Client::Get().Connected() && 
			(running_status.size() > 0 || done_results.size() > 0))
		{
			ResourcePool::Lock lk(&running_mutex);
			
			for(map<string, crack_status>::iterator it = running_status.begin(); 
				it != running_status.end(); it++)
				Client::Get().ReportStatusToServer(&it->second);
			running_status.clear();
				
			for(vector<crack_result>::iterator it = done_results.begin(); 
				it != done_results.end(); )
			{
				if(Client::Get().ReportResultToServer(&(*it)) <= 0)
					it ++;
				else
					it = done_results.erase(it);
			}
		}
				
		//从资源池获取可用的计算单元
		ResourcePool::Lock lk(pool.GetMutex());
		//prsp = pool.CoordinatorQuery(status, crack_device);
		//if(!status) continue;
		
		int k = pool.CoordinatorQuery(rs, MAX_PARALLEL_NUM, crack_device);
		if(k == 0) continue;
		status = rs[0]->m_rs_status;
		CLog::Log(LOG_LEVEL_NOMAL, "ccoordinator: CoordinatorQuery %d %s\n", k, status_msg[status]);
		
		//处理
		if(status == RS_STATUS_READY)
		{
			//这里需要从网络获取workitem数据，只有有数据才会进行下一步
			if(CrackManager::Get().CouldCrack() == false)
				continue;
				
			if(Client::Get().WillFetchItemFromServer() == false)
				continue;
			
			ret = Client::Get().GetWorkItemFromServer(&item);
			if(ret != sizeof(item))	continue;
			
			CLog::Log(LOG_LEVEL_NOMAL, "ccoordinator: Fetch workitem [guid=%s]\n", item.guid);
			if(CrackManager::Get().CheckParameters(&item) != 0)
			{
				crack_result result;
				strcpy(result.guid, item.guid);
				result.status = WI_STATUS_UNLOCK;
				Client::Get().ReportResultToServer(&result);
				continue;
			}
		
			//从服务器申请任务，并且将资源状态设置为RS_STATUS_AVAILABLE
			CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: Lock one compute unit\n");
			pool.SetToAvailable(rs, k, &item);
		}
		else if(status == RS_STATUS_RECOVERED || status == RS_STATUS_UNRECOVERED)
		{	
			//提交结果到服务器，并释放资源池
			bool report = rs[0]->m_report;
			if(report)
				CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: Submit result [guid=%s]\n", rs[0]->m_guid);
			crack_result result;
			strcpy(result.guid, rs[0]->m_guid);
			if(rs[0]->m_is_recovered){
				result.status = WI_STATUS_CRACKED;
				strncpy(result.password, rs[0]->m_password, sizeof(result.password));
			} else{
				result.status = WI_STATUS_NO_PWD;
			}
			//TODO:需要考虑如果服务器宕机，需要将解密结果持久化:-)
			if(exit_signal == 0 && report && Client::Get().ReportResultToServer(&result) <= 0)
			{
				CLog::Log(LOG_LEVEL_NOTICE, "ccoordinator: Server offline, save result[guid=%s]\n", rs[0]->m_guid);
				ResourcePool::Lock lk(&running_mutex);
				done_results.push_back(result);
			}
			pool.SetToReady(rs, k);
		}
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
