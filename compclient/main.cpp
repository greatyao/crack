#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "resourceslotpool.h"
#include "coordinator.h"
#include "launcher.h"
#include "CLog.h"
#include "CrackManager.h"
#include "Client.h"
#include "Config.h"
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
	//读取配置文件
	Config::Get().ReadConfig("compclient.conf");
	
	//初始化日志系统
	string value;
	if(Config::Get().GetValue("log_type", value) == 0 && value == "0")
		CLog::InitLogSystem(LOG_TO_SCREEN, true, NULL);
	else
	{
		Config::Get().GetValue("log_file", value);
		CLog::InitLogSystem(LOG_TO_FILE, true, value.c_str());
	}
	
	//后台运行
	if(Config::Get().GetValue("daemon", value) == 0 && value == "1")
		daemon(1, 1);

	//资源池初始化
	ResourcePool::Get().Init();
	
	//连接服务端
	string addr, port;
	Config::Get().GetValue("server_addr", addr);
	Config::Get().GetValue("server_port", port);
	Client::Get().Connect(addr.c_str(), atoi(port.c_str()));
	
	//解密算法初始化
	if(CrackManager::Get().Init() != 0)
	{
		Client::Get().Destory();
		CLog::ReleaseLogSystem();
		return -1;
	}
	
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
	Client::Get().Destory();
	
	//关闭日志系统
	CLog::ReleaseLogSystem();
	return ret;
}
