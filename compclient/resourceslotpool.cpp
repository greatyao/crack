/* resouceslotplot.cpp
 *
 * global resource plot
 * Copyright (C) 2014 TRIMPS
 *
 * Created By WANG Guofeng at  01/22/2014
 * Revisited By YAO Wei at 03/17/2014
 */
 
#include "resourceslotpool.h"
#include "CLog.h"
#include "algorithm_types.h"
#include <CL/cl.h>

#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#endif
#include <functional>

static int cpu_num;
static int gpu_num;

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
    cpu_num = proc;
    CLog::Log(LOG_LEVEL_NOMAL, "Detected %d CPUs.\n", proc);
#ifndef TEST
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
		p->m_device = 0xff00 | i;
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
	
	gpu_num = num_gpu;
	m_bIsLauncher = 0;
}
	
void ResourcePool::GetDevicesNo(int* gpu, int* cpu)
{
	if(gpu) *gpu = gpu_num;
	if(cpu) *cpu = cpu_num;
}

int ResourcePool::CoordinatorQuery(resourceslot* plots[], int n, int type)
{
	m_base_coordinator%=m_rs_pool.size();
	int i = 0, j = m_base_coordinator;
	unsigned short platformId = 0xff;
	unsigned short status0;
	
	//char text[2048] = {0};
	//for(int i = 0; i < m_rs_pool.size(); i++)
	//	sprintf(text, "%s (%04x,%d)", text, m_rs_pool[i]->m_device, m_rs_pool[i]->m_rs_status);
	//CLog::Log(LOG_LEVEL_NOMAL,"resources %s\n", text);
	
	do{
		resourceslot* p = m_rs_pool[j];
		unsigned short status = p->m_rs_status;
			
		if((type == -1 || p->m_worker_type == type) && 
			status>=RS_STATUS_READY && status<=RS_STATUS_UNRECOVERED) 
		{
			if(i == 0)
			{
				platformId = (p->m_device) >> 8;
				status0 = status;
				plots[i++] = p;
			}
			else if(status0 == status && platformId == ((p->m_device) >> 8))
			{
				plots[i++] = p;
			}	
			if(i == n) break;
		}
		
		j = (j+1) % m_rs_pool.size();
	}while(j != m_base_coordinator);
	
	m_base_coordinator = (j)% m_rs_pool.size();

	return i;
}

struct _resourceslotpool* ResourcePool::CoordinatorQuery(unsigned &u_status, int type)
{
	struct _resourceslotpool* p_rsp = 0;
	u_status = 0;
	
	//if(!m_bIsLauncher)
	{
		//处理
		m_base_coordinator%=m_rs_pool.size();
		for(; m_base_coordinator<m_rs_pool.size(); m_base_coordinator++)
		{
			struct _resourceslotpool* p = m_rs_pool[m_base_coordinator];
			if((type == -1 || p->m_worker_type == type) && 
				p->m_rs_status>=RS_STATUS_READY && p->m_rs_status<=RS_STATUS_UNRECOVERED) 
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

int ResourcePool::LauncherQuery(resourceslot* plots[], int n)
{
	m_base_launcher%=m_rs_pool.size();
	int i = 0, j = m_base_launcher;
	unsigned short status0;
	unsigned short platformId = 0xff;
	do{
		resourceslot* p = m_rs_pool[j];
		unsigned short status = p->m_rs_status;
		if( (status==RS_STATUS_AVAILABLE)||(status==RS_STATUS_FAILED) )
		{
			if(i == 0)
			{
				platformId = (p->m_device) >> 8;
				status0 = status;
				plots[i++] = p;
			}
			else if(status0 == status && platformId == ((p->m_device) >> 8))
			{
				plots[i++] = p;
			}	
			if(i == n) break;
		}
		
		j = (j+1) % m_rs_pool.size();
	}while(j != m_base_launcher);
	
	m_base_launcher = (j)% m_rs_pool.size();

	return i;
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

struct _resourceslotpool* ResourcePool::QueryByGuid(const char* guid)
{
	struct _resourceslotpool* p_rsp = 0;
	for(int i = 0; i<m_rs_pool.size(); i++)
	{
		struct _resourceslotpool* p = m_rs_pool[i];
		if(strcmp(guid, p->m_guid) == 0)
			return p;
	}
	
	return p_rsp;
}

	
int ResourcePool::QueryByGuid(resourceslot* plots[], int n, const char* guid)
{
	int j = 0;
	for(int i = 0; i<m_rs_pool.size(); i++)
	{
		struct _resourceslotpool* p = m_rs_pool[i];
		if(strcmp(guid, p->m_guid) == 0)
		{
			plots[j++] = p;
		}
	}
	
	return j;
}

void ResourcePool::SetToReady(struct _resourceslotpool*p)
{
	p->m_rs_status = RS_STATUS_READY;
	if(p->m_item){
		free(p->m_item);
		p->m_item = 0;
	}
	memset(p->m_guid, 0, sizeof(p->m_guid));
}

void ResourcePool::SetToOccupied(struct _resourceslotpool*p)
{
	p->m_rs_status = RS_STATUS_OCCUPIED;
}

void ResourcePool::SetToFailed(struct _resourceslotpool*p)
{
	p->m_rs_status = RS_STATUS_FAILED;
}

void ResourcePool::SetToAvailable(struct _resourceslotpool*p, crack_block* item)
{
	p->m_rs_status = RS_STATUS_AVAILABLE;
	if(p->m_item)	free(p->m_item);
	p->m_item = (crack_block*)malloc(sizeof(crack_block));
	memcpy(p->m_item, item, sizeof(crack_block));
	strcpy(p->m_guid, item->guid);
}

void ResourcePool::SetToRecover(struct _resourceslotpool* p, bool cracked, const char* passwd, bool report)
{
	p->m_rs_status = cracked ? RS_STATUS_RECOVERED : RS_STATUS_UNRECOVERED;
	p->m_is_recovered = cracked;
	p->m_report = report;
	if(cracked)
		strncpy(p->m_password, passwd, sizeof(p->m_password));
}

void ResourcePool::SetToReady(resourceslot* plots[], int n)
{
	for(int i = 0; i < n; i++)
		SetToReady(plots[i]);
}

void ResourcePool::SetToOccupied(resourceslot* plots[], int n)
{
	for(int i = 0; i < n; i++)
		SetToOccupied(plots[i]);
}

void ResourcePool::SetToFailed(resourceslot* plots[], int n)
{
	for(int i = 0; i < n; i++)
		SetToFailed(plots[i]);
}

void ResourcePool::SetToAvailable(resourceslot* plots[], int n, crack_block* item)
{
	for(int i = 0; i < n; i++)
		SetToAvailable(plots[i], item);
}

void ResourcePool::SetToRecover(resourceslot* plots[], int n, bool cracked, const char* passwd, bool report)
{
	for(int i = 0; i < n; i++)
		SetToRecover(plots[i], cracked, passwd, report);
}

void ResourcePool::SaveOneDone(struct crack_result* result)
{
	struct crack_result* cp = new struct crack_result(*result);
	m_done_results.push_back(cp);
}

int ResourcePool::GetDoneSize()const
{
	return m_done_results.size();
}