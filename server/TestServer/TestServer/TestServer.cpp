// TestServer.cpp : Defines the class behaviors for the application.
//

#include "SockServer.h"
#include "CrackBroker.h"
#include "CLog.h"
#include <string>
using std::string;

static int logtype = LOG_TO_SCREEN;
static int loglevel = LOG_LEVEL_NOMAL;
static int port = 6010;
static int use_leveldb = false;

inline void printUsage(const char* exec)
{
    fprintf(stderr, "Usage: %s [--port=port] [--logtype=type] "
					"[--loglevel=level] [--help]\n\n"
					"  --port\tServer binding port\n"
					"  --logtype\t1:file, 2:screen\n"
					"  --loglevel\tlog level [0-5]\n"
					"  --leveldb\tUse levelDB to storage task\n"
					"  --help\t\tprints this help message.\n", exec);
}

void parseOptions(int argc, char ** argv)
{
	if(argc == 1)
		return;

    if ((string)argv[1] == "--help")
    {
        printUsage(argv[0]);
        exit(EXIT_SUCCESS);
    }

    string temp;
    for (int i = 1; i < argc; ++i)
    {
        temp = (string)argv[i];
        if (temp.substr(0, 7) == "--port=")
        {
            port = atoi(temp.substr(7).c_str());
        }
        else if (temp.substr(0, 10) == "--logtype=")
        {
            logtype = atoi(temp.substr(10).c_str());
        }
        else if (temp.substr(0, 11) == "--loglevel=")
        {
            loglevel = atoi(temp.substr(11).c_str());
        }
		else if (temp.substr(0, 9) == "--leveldb")
        {
            use_leveldb = true;
        }
    }
}

int main(int argc, char* argv[]){

	parseOptions(argc, argv);

	CLog::InitLogSystem(logtype, TRUE,"ScheduleServer.log");
	CLog::SetLevel(loglevel);

	g_CrackBroker.LoadFromPersistence(!use_leveldb);

	CSockServer *g_Server = new CSockServer;

	int ret = g_Server->Initialize(port,10);
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
