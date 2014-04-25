/* resouceslotplot.h
 *
 * global resource plot
 * Copyright (C) 2014 TRIMPS
 *
 * Created By WANG Guofeng at  01/22/2014
 */
 
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

static const int MAX_PARALLEL_NUM = 6;

enum{
	RS_STATUS_UNDEFINED,	//未定义
	
	//Coordinator
	RS_STATUS_READY,	//RS空闲状态（就绪状态），可以让Coordinator向服务器申请解密任务
	RS_STATUS_RECOVERED,	//解密任务处理成功，需要将结果上传
	RS_STATUS_UNRECOVERED,	//解密结束

	//Launcher
	RS_STATUS_AVAILABLE,//Coordinator已经将解密任务放置好，等待Launcher处理
	RS_STATUS_FAILED,	//解密任务处理失败，Launcher将其重置到READY状态
	RS_STATUS_OCCUPIED, //正在处理解密任务,RS_STATUS_AVAILABLE之后设置的状态

	//other
	RS_STATUS_MAX
};

static const char* status_msg[] = {"undefined", "ready", "recoverd", "unrecovered",
									"available", "failed", "occupied", "unknown"};

enum{
	DEVICE_CPU,  //CPU Worker
	DEVICE_GPU,  //GPU Worker
	DEVICE_FPGA  //FPGA Worker
}; 

struct crack_block;
struct crack_result;

typedef struct _resourceslotpool
{
	char		    m_guid[40];				//解密单元工作者编号
	unsigned short	m_worker_type;			//工作者类型，CPU,GPU,FPGA
	unsigned short	m_device;				//GPU的platform|设备ID/CPU的ID
	unsigned short	m_rs_status;			//当前slot状态
	unsigned short  m_progress;				//workitem对应的进度
	bool	 	    m_b_islocked;			//互斥锁
	bool 		    m_is_recovered;			//密码是否恢复成功
	bool			m_report;				//是否上报服务端
	char		    m_password[32];			//如果解密成功，这里保存密码明文
	crack_block*	m_item;					//解密的workitem，需要动态分配
	unsigned short  m_shared;				//是否与其他device的共享
}resourceslot;

class ResourcePool
{
private:
	bool	m_b_inited;
	pthread_mutex_t mutex;
	
	vector <struct _resourceslotpool *> m_rs_pool;//存放所有的计算资源
	vector <struct crack_result *> m_done_results;//存放计算结束的资源，必要时清空

	unsigned m_base_coordinator;
	unsigned m_base_launcher;

	ResourcePool();
	~ResourcePool();
public:

	class Lock{
	private:
		pthread_mutex_t* _m;
	public:
		Lock(pthread_mutex_t* m):_m(m){pthread_mutex_lock(_m);}
		~Lock(){pthread_mutex_unlock(_m);}
	};

	static ResourcePool& Get();

	void Init();
		
	pthread_mutex_t* GetMutex(){return &mutex;}
	/***************************************************************
	轮流查询机制
	***************************************************************/
	unsigned m_bIsLauncher;
	
	//获取CPU和GPU设备数目
	void GetDevicesNo(int* gpu, int* cpu);

	//Coordinator查询接口(-1表示不限类型)
	//获取多个资源（注意所有的资源状态必须一致）
	int CoordinatorQuery(resourceslot* plots[], int n, int type = -1);
	
	//Launcher查询接口
	int LauncherQuery(resourceslot* plots[], int n);
	
	int QueryByGuid(resourceslot* plots[], int n, const char* guid);
	
	void SaveOneDone(struct crack_result* result);
	
	template <typename Function> 
	void ReportDoneAgain(Function func)
	{
		typedef vector <struct crack_result *>::iterator iterator;
		for(iterator it = m_done_results.begin(); it != m_done_results.end(); )
		{
			if(func(*it) <= 0)
				it ++;
			else
				it = m_done_results.erase(it);
		}
	}
	
	int GetDoneSize()const;
		
	/***************************************************************
	处理接口
	***************************************************************/
	void SetToReady(struct _resourceslotpool*);
	void SetToOccupied(struct _resourceslotpool*);
	void SetToFailed(struct _resourceslotpool*);
	void SetToAvailable(struct _resourceslotpool*, crack_block* item);
	void SetToRecover(struct _resourceslotpool*, bool cracked, const char* passwd, bool report);
	
	void SetToReady(resourceslot* plots[], int n);
	void SetToOccupied(resourceslot* plots[], int n);
	void SetToFailed(resourceslot* plots[], int n);
	void SetToAvailable(resourceslot* plots[], int n, crack_block* item);
	void SetToRecover(resourceslot* plots[], int n, bool cracked, const char* passwd, bool report);
	
	//可以增加其他处理函数。
};

#endif
