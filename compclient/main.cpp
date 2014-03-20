#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "resourceslotpool.h"
#include "coordinator.h"
#include "launcher.h"
#include "CLog.h"
#include "Client.h"
#include "loadfiles.h"
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
	signal(SIGTERM, handle_signal); //* �������������źŵĴ����� 
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
	//��ʼ����־ϵͳ��һ������ֻ��Ҫ�������ʼ��һ�Σ�
	if(argc == 1)
		CLog::InitLogSystem(LOG_TO_FILE, true, "sys.log");
	else
		CLog::InitLogSystem(LOG_TO_SCREEN, true, NULL);
#if 0		
	struct crack_hash hashes[32];
	load_single_hash("63cb5261f4610ba648fcd5e1b72c3173", algo_md5, hashes);
	printf("%s\n", hashes[0].hash);
	
	int n = load_hashes_file("md5.hashes", algo_md5, hashes, sizeof(hashes));
	for(int i = 0; i < n; i++)
		printf("%d: %s\n", i, hashes[i].hash);
#endif	
		
	Client::Get().Connect("192.168.18.117", 5150);

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

	int ret = main_loop();
	
	//����������
	delete pcc;
	delete pcl;

	//�ر���־ϵͳ
	CLog::ReleaseLogSystem();
	return ret;
}
