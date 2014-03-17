/***************************************************************
Coordinator,���༭ 2014��1��23��
***************************************************************/
#ifndef __COORDINATOR__H___
#define __COORDINATOR__H___

#include <string>
#include <vector>

#include "pthread.h"

using namespace std;

/***************************************************************
Coordinator
�������Դ�ز�ѯ״̬�����ͷ������˽�������
��Ҫ�������߳�
***************************************************************/
class ccoordinator
{
	pthread_t m_pThread;
	int m_bStop;
	int m_bThreadRunning;

public:
	ccoordinator();
	~ccoordinator();
	
	static void *Thread(void *);	//ɨ���߳�
	void Start(void);		//��ʼɨ���߳�
	void Stop(void);		//ֹͣɨ���߳�
};


#endif
