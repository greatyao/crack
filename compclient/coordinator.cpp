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

void *ccoordinator::Thread(void*par)//ɨ���߳� + ��socket��ȡitem
{
	ccoordinator *p = (ccoordinator*)par;
	struct _resourceslotpool *prsp;
	unsigned uStatus = 0;
	
	while(1)
	{
		if(p->m_bStop) break;
	
		//������Ҫ�������ȡworkitem���ݣ�ֻ�������ݲŻ������һ��

		//do
		ResourcePool::Get().Lock();
		prsp = ResourcePool::Get().CoordinatorQuery(uStatus);

		if(!uStatus)
		{
			CLog::Log(LOG_LEVEL_WARNING,"ccoordinator: find non resource\n");
			goto next;
		}
		
		//����
		switch(uStatus)
		{
			case RS_STATUS_READY:
				{
					//�ӷ������������񣬲��ҽ���Դ״̬����ΪRS_STATUS_AVAILABLE
					CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: allocate one resource\n");
					ResourcePool::Get().SetToAvailable(prsp);
				}
				break;
			case RS_STATUS_RECOVERED:
				{	
					//�ύ����������������ͷ���Դ��
					CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: submit result\n");
					ResourcePool::Get().SetToReady(prsp);
				}
				break;
			case RS_STATUS_UNRECOVERED:
				{	
					//�ύ����������������ͷ���Դ��
					CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator: submit result\n");
					ResourcePool::Get().SetToReady(prsp);
				}
				break;
			default:
				break;
		}
next:	
		ResourcePool::Get().UnLock();
		Sleep(3000);
	}
	
	fprintf(stderr, "leaving %s\n", __FUNCTION__);
	return 0;
}
void ccoordinator::Start(void)//��ʼɨ���߳�
{	
	if(m_bThreadRunning)
	{
		CLog::Log(LOG_LEVEL_WARNING,"ccoordinator ɨ���߳������У�����Ҫ�ٴ���\n");
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
void ccoordinator::Stop(void)//ֹͣɨ���߳�
{
	if(m_bThreadRunning==0)
	{
		CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator û��ɨ���߳�������\n");
		return;
	}

	m_bStop = 1;
	int returnValue = pthread_join(m_pThread, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"ccoordinator �߳��˳�ʧ�ܣ��������: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"ccoordinator ɨ���̳߳ɹ��˳�\n");
	}
	m_bThreadRunning = 0;
}
