/***************************************************************
资源池,最后编辑 2014年1月22日
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

enum{
	DEVICE_CPU,  //CPU Worker
	DEVICE_GPU,  //GPU Worker
	DEVICE_FPGA  //FPGA Worker
}; 

struct crack_block;

struct _resourceslotpool
{
	char		    m_guid[40];				//解密单元工作者编号
	unsigned short	m_worker_type;			//工作者类型，CPU,GPU,FPGA
	unsigned short	m_device;				//GPU的platform|设备ID/CPU的ID
	unsigned short	m_rs_status;			//当前slot状态
	unsigned short  m_progress;				//workitem对应的进度
	bool	 	    m_b_islocked;			//互斥锁
	bool 		    m_is_recovered;			//密码是否恢复成功
	string		    m_string_pars;			//解密单元参数
	char		    m_password[32];			//如果解密成功，这里保存密码明文
	crack_block*	m_item;					//解密的workitem，需要动态分配
	unsigned short  m_shared;				//是否与其他device的共享
};

class ResourcePool
{
private:
	bool	m_b_inited;
	pthread_mutex_t mutex;
	
	vector <struct _resourceslotpool *> m_rs_pool;//存放所有的计算资源
	vector <struct _resourceslotpool *> m_done_pool;//存放计算结束的资源，必要时清空

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
	轮流查询机制
	***************************************************************/
	unsigned m_bIsLauncher;

	//Coordinator查询接口
	struct _resourceslotpool* CoordinatorQuery(unsigned &u_status);
	
	//Launcher查询接口
	struct _resourceslotpool* LauncherQuery(unsigned &u_status);
	
	struct _resourceslotpool* QueryByGuid(const char* guid);
		
	/***************************************************************
	处理接口
	***************************************************************/
	void SetToReady(struct _resourceslotpool*);
	void SetToOccupied(struct _resourceslotpool*);
	void SetToAvailable(struct _resourceslotpool*, crack_block* item);
	void SetToRecover(struct _resourceslotpool*, bool cracked, const char* passwd);
	
	//可以增加其他处理函数。
};

#endif
