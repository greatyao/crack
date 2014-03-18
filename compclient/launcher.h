/***************************************************************
Launcher,最后编辑 2014年1月23日
***************************************************************/
#ifndef __LAUNCHER__H___
#define __LAUNCHER__H___

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#endif

#include <string>
#include <vector>

#include "pthread.h"

using namespace std;

/***************************************************************
Launcher
负责从资源池查询任务，并调用解密插件执行任务
需要开独立线程
***************************************************************/
class clauncher
{
	pthread_t m_pThread;
	bool m_bStop;
	bool m_bThreadRunning;
public:
	clauncher();
	~clauncher();
	
	static void *Thread(void *);//扫描线程
	void Start(void);			//开始扫描线程
	void Stop(void);			//停止扫描线程
	
	static int ReportDone(char* guid, bool cracked, const char* passwd);

	static int ReportStatus(char* guid, int progress, float speed, unsigned int remainTime);
};

#endif
