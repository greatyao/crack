#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "coordinator.h"
#include "launcher.h"
#include "CLog.h"
#include "CrackManager.h"
#include "algorithm_types.h"

#if defined(__CYGWIN__) || defined(__linux__)
#include <unistd.h>
#endif

int exit_signal = 0;

static void handle_signal(int sig)
{
	if(sig == SIGTERM || sig == SIGINT || sig == SIGQUIT){
		CLog::Log(LOG_LEVEL_WARNING, "Caught signal %d, exiting\n", sig);
		exit_signal = 1;
	} else if(sig == SIGPIPE){
		CLog::Log(LOG_LEVEL_WARNING, "Caught pipe %d\n", sig);
	}
}

static int main_loop()
{
	signal(SIGTERM, handle_signal); //* 下面设置三个信号的处理方法 
	signal(SIGINT, handle_signal); 
	signal(SIGQUIT, handle_signal); 
	signal(SIGPIPE, handle_signal); 
	
	int j = 0;
	while(!exit_signal) {
		sleep(2);
	}
	
	return exit_signal;
}

int main(int argc, char *argv[])
{
	//解密管理单元初始化
	if(CrackManager::Get().Init() != 0)
		return -1;
	
	//初始化coordinator
	ccoordinator *pcc = new ccoordinator();
	//初始化launcher
	clauncher *pcl = new clauncher();
	//启动coordinator
	pcc->Start();
	//启动launcher
	pcl->Start();

	int ret = main_loop();
	
	//结束，清理
	delete pcc;
	delete pcl;
	CrackManager::Get().Destroy();

	return ret;
}
