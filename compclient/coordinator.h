/***************************************************************
Coordinator,最后编辑 2014年1月23日
***************************************************************/
#ifndef __COORDINATOR__H___
#define __COORDINATOR__H___

#include <string>
#include <vector>

#include "pthread.h"

using namespace std;

/***************************************************************
Coordinator
负责从资源池查询状态，并和服务器端交互数据
需要开独立线程
***************************************************************/
class ccoordinator
{
	pthread_t m_pThread;
	int m_bStop;
	int m_bThreadRunning;

public:
	ccoordinator();
	~ccoordinator();
	
	static void *Thread(void *);	//扫描线程
	void Start(void);		//开始扫描线程
	void Stop(void);		//停止扫描线程
};


#endif
