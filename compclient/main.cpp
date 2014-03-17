//#include <windows.h>
#include <stdio.h>
#include "resourceslotpool.h"
#include "coordinator.h"
#include "launcher.h"
#include "CLog.h"
#if defined(__CYGWIN__) || defined(__linux__)
#include <unistd.h>
#endif

int main(int argc, char *argv[])
{
	//��ʼ����־ϵͳ��һ������ֻ��Ҫ�������ʼ��һ�Σ�
	if(argc == 1)
		CLog::InitLogSystem(LOG_TO_FILE, true, "sys.log");
	else
		CLog::InitLogSystem(LOG_TO_SCREEN, true, NULL);

	//������Դ����
	ResourcePool::Get().Init();
	
	//cclient *pcomp = new cclient(); 
	//��ʼ��coordinator
	ccoordinator *pcc = new ccoordinator();
	//��ʼ��launcher
	clauncher *pcl = new clauncher();
	//����coordinator
	pcc->Start();
	//����launcher
	pcl->Start();

	//���������״̬
	//docmd(pcomp);
	while(1)
		sleep(1);
	
	//����������
	delete pcc;
	//delete pcl;

	//delete pcomp;

	//�ر���־ϵͳ
	//CLog::ReleaseLogSystem();
	return 0;
}
