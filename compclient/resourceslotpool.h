/***************************************************************
��Դ��,���༭ 2014��1��22��
***************************************************************/
#ifndef __RS_POOL__H___
#define __RS_POOL__H___

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#endif
#include <string>
#include <vector>
#include <pthread.h>
using namespace std;


enum{
	RS_STATUS_UNDEFINED,	//δ����
	
	//Coordinator
	RS_STATUS_READY,	//RS����״̬������״̬����������Coordinator������������������
	RS_STATUS_RECOVERED,	//����������ɹ�����Ҫ������ϴ�
	RS_STATUS_UNRECOVERED,	//���ܽ���

	//Launcher
	RS_STATUS_AVAILABLE,//Coordinator�Ѿ�������������úã��ȴ�Launcher����
	RS_STATUS_FAILED,	//����������ʧ�ܣ�Launcher�������õ�READY״̬
	RS_STATUS_OCCUPIED, //���ڴ����������,RS_STATUS_AVAILABLE֮�����õ�״̬

	//other
	RS_STATUS_MAX
};

enum{
	DEVICE_CPU,  //CPU Worker
	DEVICE_GPU,  //GPU Worker
	DEVICE_FPGA  //FPGA Worker
}; 

struct crack_block;

struct _resourceslotpool
{
	char		    m_guid[40];				//���ܵ�Ԫ�����߱��
	unsigned short	m_worker_type;			//���������ͣ�CPU,GPU,FPGA
	unsigned short	m_device;				//GPU��platform|�豸ID/CPU��ID
	unsigned short	m_rs_status;			//��ǰslot״̬
	unsigned short  m_progress;				//workitem��Ӧ�Ľ���
	bool	 	    m_b_islocked;			//������
	bool 		    m_is_recovered;			//�����Ƿ�ָ��ɹ�
	string		    m_string_pars;			//���ܵ�Ԫ����
	char		    m_password[32];			//������ܳɹ������ﱣ����������
	crack_block*	m_item;					//���ܵ�workitem����Ҫ��̬����
	unsigned short  m_shared;				//�Ƿ�������device�Ĺ���
};

class ResourcePool
{
private:
	bool	m_b_inited;
	pthread_mutex_t mutex;
	
	vector <struct _resourceslotpool *> m_rs_pool;//������еļ�����Դ
	vector <struct _resourceslotpool *> m_done_pool;//��ż����������Դ����Ҫʱ���

	unsigned m_base_coordinator;
	unsigned m_base_launcher;

	ResourcePool();
	~ResourcePool();
public:

	static ResourcePool& Get();

	void Init();
		
	void Lock(void);
	void UnLock(void);	
	/***************************************************************
	������ѯ����
	***************************************************************/
	unsigned m_bIsLauncher;

	//Coordinator��ѯ�ӿ�
	struct _resourceslotpool* CoordinatorQuery(unsigned &u_status);
	
	//Launcher��ѯ�ӿ�
	struct _resourceslotpool* LauncherQuery(unsigned &u_status);
	
	struct _resourceslotpool* QueryByGuid(const char* guid);
		
	/***************************************************************
	����ӿ�
	***************************************************************/
	void SetToReady(struct _resourceslotpool*);
	void SetToOccupied(struct _resourceslotpool*);
	void SetToAvailable(struct _resourceslotpool*, crack_block* item);
	void SetToRecover(struct _resourceslotpool*, bool cracked, const char* passwd);
	
	//��������������������
};

#endif
