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
	//初始化日志系统（一个程序，只需要在最初初始化一次）
	if(argc == 1)
		CLog::InitLogSystem(LOG_TO_FILE, true, "sys.log");
	else
		CLog::InitLogSystem(LOG_TO_SCREEN, true, NULL);

	//申请资源池类
	ResourcePool::Get().Init();
	
	//cclient *pcomp = new cclient(); 
	//初始化coordinator
	ccoordinator *pcc = new ccoordinator();
	//初始化launcher
	clauncher *pcl = new clauncher();
	//启动coordinator
	pcc->Start();
	//启动launcher
	pcl->Start();

	//进入命令处理状态
	//docmd(pcomp);
	while(1)
		sleep(1);
	
	//结束，清理
	delete pcc;
	//delete pcl;

	//delete pcomp;

	//关闭日志系统
	//CLog::ReleaseLogSystem();
	return 0;
}
