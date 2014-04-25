// TestServer.cpp : Defines the class behaviors for the application.
//

#include "SockServer.h"
#include "CLog.h"

int main(int argc, char* argv[]){
	CLog::InitLogSystem(LOG_TO_SCREEN, TRUE,"ScheduleServer.log");

	CSockServer *g_Server = new CSockServer;

	int ret = g_Server->Initialize(6010,10);
	if (ret < 0 ){
	
		CLog::Log(LOG_LEVEL_WARNING,"initalize error %d\n",ret);

	}
	ret = g_Server->StartServer();
	if (ret != 0 ){


		CLog::Log(LOG_LEVEL_WARNING,"start server %d\n",ret);
		return FALSE;

	}

	
	g_Server->ShutDown();


	return 0;
}
