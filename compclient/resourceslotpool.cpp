#include "resourceslotpool.h"
#include <CL/cl.h>
#include "CLog.h"

#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#endif

ResourcePool::ResourcePool()
{	
	m_base_coordinator = 0;
	m_base_launcher    = 0;
	m_b_inited = 0;
	pthread_mutex_init(&mutex, NULL);
}

ResourcePool::~ResourcePool()
{
	pthread_mutex_destroy(&mutex);
}

ResourcePool& ResourcePool::Get()
{
	static ResourcePool instance;
	return instance;
}

static unsigned int get_num_cpu(void)
#if defined(WIN32) || defined(WIN64)
{
	return 2;
}
#else
/* Get number of CPUs, read from /proc/cpuinfo */
{
    FILE *fcpu;
    unsigned int proc = 0;
    char buf[128];
    char *status;

    bzero(buf,128); // Make valgrind happy
    fcpu=fopen("/proc/cpuinfo","r");
    if (!fcpu)
    {
        CLog::Log(LOG_LEVEL_WARNING, "Cannot open /proc/cpuinfo! errno=%d", errno);
        return 0;
    }
    else
    {
        do
        {
            status = fgets(buf, 128, fcpu);
            if ((status) && (strstr(buf, "processor"))) proc++;
        } while (status);

        fclose(fcpu);
    }
    CLog::Log(LOG_LEVEL_NOMAL, "Detected %d CPUs.\n", proc);
#ifdef TEST
	return 1;
#else
    return proc;
#endif
}
#endif

void ResourcePool::Init()
{
	unsigned num_cpu = get_num_cpu();

	if(num_cpu>0) 
	{
		m_b_inited =1;
	}
	else
	{
		return;
	}
	
	for(unsigned i=0; i<num_cpu; i++)
	{
		struct _resourceslotpool *p = (_resourceslotpool *)malloc(sizeof(*p));
		memset(p, 0, sizeof(*p));
		p->m_worker_type = DEVICE_CPU;
		p->m_device = i;
		p->m_rs_status = RS_STATUS_READY;
		m_rs_pool.push_back(p);
	}

	unsigned num_gpu = 0;
	cl_device_id device_id[16];
    cl_uint num_of_devices;
    cl_int err;
    cl_platform_id platforms[4];
	cl_uint num_platforms = 0;
	char platformname[256];
	char devicename[255];
    char devicevendor[255];

	err = clGetPlatformIDs(4, platforms, &num_platforms);
	if(err != CL_SUCCESS){
		CLog::Log(LOG_LEVEL_WARNING, "Detected Non OpenCL platform!\n");
		return;
	}

    for(unsigned int i=0;i<num_platforms;i++)
    {
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION,255, platformname, NULL);
		err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 16, device_id, &num_of_devices);
		if (err!=CL_SUCCESS) continue;
		for (int a=0;a<num_of_devices;a++)
		{
			switch (err)
			{
			case CL_INVALID_PLATFORM:
				CLog::Log(LOG_LEVEL_WARNING, "Invalid OpenCL platform!%s\n","");
				break;
			case CL_DEVICE_NOT_FOUND:
				CLog::Log(LOG_LEVEL_WARNING, "No suitable GPU devices found!%s\n","");
				break;
			default: 
				clGetDeviceInfo( device_id[a], CL_DEVICE_NAME, 254, devicename,  NULL);
				clGetDeviceInfo( device_id[a], CL_DEVICE_VENDOR, 254, devicevendor,  NULL);
				CLog::Log(LOG_LEVEL_NOMAL, "Found GPU device: %s - %s\n", devicevendor,devicename);
				struct _resourceslotpool *p = (_resourceslotpool *)malloc(sizeof(*p));
				memset(p, 0, sizeof(*p));
				p->m_worker_type = DEVICE_GPU;
				p->m_device = (i<<8)|a;
				p->m_rs_status = RS_STATUS_READY;
				m_rs_pool.push_back(p);
				
				num_gpu++;
				break;
			}
		}
	}
		    
	m_bIsLauncher = 0;
}
	
void ResourcePool::Lock(void)
{
	pthread_mutex_lock(&mutex);
}
void ResourcePool::UnLock(void)
{
	pthread_mutex_unlock(&mutex);
}

struct _resourceslotpool* ResourcePool::CoordinatorQuery(unsigned &u_status)
{
	struct _resourceslotpool* p_rsp = 0;
	u_status = 0;
	
	//printf("%s %d\n", __FUNCTION__, m_base_coordinator);

	//if(!m_bIsLauncher)
	{
		//处理
		m_base_coordinator%=m_rs_pool.size();
		for(; m_base_coordinator<m_rs_pool.size(); m_base_coordinator++)
		{
			struct _resourceslotpool* p = m_rs_pool[m_base_coordinator];
			if(p->m_rs_status==RS_STATUS_READY)
			{
				u_status = p->m_rs_status;
				p_rsp = p;
				break;
			}
		}
		if(m_base_coordinator!=m_rs_pool.size())m_base_coordinator++;

		m_bIsLauncher = 1;
	}
	return p_rsp;
}

struct _resourceslotpool* ResourcePool::LauncherQuery(unsigned &u_status)
{
	struct _resourceslotpool* p_rsp = 0;
	u_status = 0;

	//if(m_bIsLauncher)
	{
		//处理
		m_base_launcher%=m_rs_pool.size();
		for(; m_base_launcher<m_rs_pool.size(); m_base_launcher++)
		{
			struct _resourceslotpool* p = m_rs_pool[m_base_launcher];
			if( (p->m_rs_status==RS_STATUS_AVAILABLE)||(p->m_rs_status==RS_STATUS_FAILED) )
			{
				u_status = p->m_rs_status;
				p_rsp = p;
				break;
			}
		}
		if(m_base_launcher!=m_rs_pool.size()) m_base_launcher++;
		m_bIsLauncher = 0;
	}
	return p_rsp;
}
	
void ResourcePool::SetToReady(struct _resourceslotpool*p)
{
	p->m_rs_status = RS_STATUS_READY;
}

void ResourcePool::SetToOccupied(struct _resourceslotpool*p)
{
	p->m_rs_status = RS_STATUS_OCCUPIED;
}

void ResourcePool::SetToAvailable(struct _resourceslotpool*p)
{
	p->m_rs_status = RS_STATUS_AVAILABLE;
}
