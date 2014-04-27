/* Crack.h
 *
 * Abstarct class for crack algorithms
 * Copyright (C) 2014 TRIMPS
 *
 * Craeted By YAO Wei at  03/17/2014
 */
 
#ifndef _CRACK_H_
#define _CRACK_H_

#include <map>
#include <string>
#include <pthread.h>

struct crack_block;
class Crack;

struct lauch_param
{
	int pid;
	pthread_t tid;
	int read_fd;
	int write_fd;
	int progress;
	float speed;
	unsigned int cost_time;
	unsigned int retain_time;
};

struct thread_param
{
	Crack* crack;
	char guid[40];
};

typedef int (*ProcessDone)(char* guid, bool cracked, const char* passwd, bool report);
typedef int (*ProgressStatus)(char* guid, int progress, float speed, unsigned int remainTime);

class Crack
{
protected:
	Crack(void);
	virtual ~Crack(void);
	int Exec(const char* guid, const char* path, const char* params, void* (*monitor)(void*), 
				bool dupStdin = true, bool dupStdout = true, bool dupStderr = false);
	int Kill(const char* guid);
	int CleanUp(const char* guid);
	int UpdateStatus(const char* guid, int progress, float speed, unsigned int elapseTime, unsigned int remainTime);
	int SetPath(const char* path);
	void SetToolName(const char* name);
	int RunningTasks()const;
	
	char path[256];
	char toolname[32];
	pthread_rwlock_t rwlock;
	std::map<std::string, struct lauch_param> running;
	ProcessDone doneFunc;
	ProgressStatus statusFunc;
	friend class CrackManager;

public:
	int StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceIds[], int ndevices);
	int StopCrack(const char* guid);
	
	//注册回调函数
	void RegisterCallback(ProcessDone done, ProgressStatus status);
	
	//读取调用进程的stdout和stderr的输出信息
	//非阻塞, -1表示没有数据， 0表示进程已经结束，其他表示实际读入的数据量
	int ReadFromLancher(const char* guid, char* buf, int n);
	
	//往调用进程里面写信息：模拟调用进程的stdin
	//非阻塞, 0表示进程已经结束，其他表示实际写入的数据量
	int WriteToLancher(const char* guid, const char* buf, int n);

	//以下的虚函数需要在现每个解密软件实现
	
	//启动
	virtual int Launcher(const crack_block* item, bool gpu, unsigned short* deviceIds, int ndevices) = 0;
	
	virtual int SupportMultiTasks()const = 0;	
};

#endif
