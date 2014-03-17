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

void *clauncher::Thread(void*par)//ɨ���߳�
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
		
		//����
		switch(uStatus)
		{
			case RS_STATUS_AVAILABLE:
				{
					//�ύ�����ܲ��ִ�У�ִ���������ִ�н��
					CLog::Log(LOG_LEVEL_NOMAL,"clauncher: doing crack task\n");
					ResourcePool::Get().SetToOccupied(prsp);

					//���ý��ܲ��ִ�н�������
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
				{	//���³�ʼ����Դ�أ����ͷ���Դ��
					CLog::Log(LOG_LEVEL_NOMAL,"clauncher: find failed task\n");
					ResourcePool::Get().SetToReady(prsp);
				}
				break;
			//case RS_STATUS_OCCUPIED://�ɽ��ܲ���ύ���ܽ���Ժ���
			//	{//����ִ�н�������
			//		CLog::Log(LOG_LEVEL_NOMAL,"clauncher ��������ִ����\n");
			//		p->m_pcrsp->SetToReady(prsp);
			//	}
				break;
			default:break;
		}
next:
		ResourcePool::Get().UnLock();
		//���ȴ�һ�£�����������ԣ�ʵ�ʲ���Ҫ
		Sleep(3000);
	}
	
	fprintf(stderr, "leaving %s\n", __FUNCTION__);
	
	return 0;
}
void clauncher::Start(void)//��ʼɨ���߳�
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
void clauncher::Stop(void)//ֹͣɨ���߳�
{
	if(m_bThreadRunning==false)
	{
		return;
	}

	m_bStop = true;
	int returnValue = pthread_join(m_pThread, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"clauncher �߳��˳�ʧ�ܣ��������: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"clauncher ɨ���̳߳ɹ��˳�\n");
	}
	m_bThreadRunning = false;
}
