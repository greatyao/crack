/***************************************************************
Launcher,���༭ 2014��1��23��
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
�������Դ�ز�ѯ���񣬲����ý��ܲ��ִ������
��Ҫ�������߳�
***************************************************************/
class clauncher
{
	pthread_t m_pThread;
	bool m_bStop;
	bool m_bThreadRunning;
public:
	clauncher();
	~clauncher();
	
	static void *Thread(void *);//ɨ���߳�
	void Start(void);			//��ʼɨ���߳�
	void Stop(void);			//ֹͣɨ���߳�
	
	static int ReportDone(char* guid, bool cracked, const char* passwd);

	static int ReportStatus(char* guid, int progress, float speed, unsigned int remainTime);
};

#endif
