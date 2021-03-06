﻿/* Crack.cpp
 *
 * Abstarct class for crack algorithms
 * Copyright (C) 2014 TRIMPS
 *
 * Craeted By YAO Wei at  03/17/2014
 */
 
#include "Crack.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#if defined(WIN32) || defined(WIN64)
#else
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#endif

class RLock
{
private:
	pthread_rwlock_t* rwlock;
public:
	RLock(pthread_rwlock_t* lock):rwlock(lock)
	{
		pthread_rwlock_rdlock(rwlock);
	}
	
	~RLock()
	{
		pthread_rwlock_unlock(rwlock);
	}

};

class WLock
{
private:
	pthread_rwlock_t* rwlock;
public:
	WLock(pthread_rwlock_t* lock):rwlock(lock)
	{
		pthread_rwlock_wrlock(rwlock);
	}
	
	~WLock()
	{
		pthread_rwlock_unlock(rwlock);
	}

};

Crack::Crack(void)
{
	this->doneFunc = NULL;
	this->statusFunc = NULL;
	pthread_rwlock_init(&rwlock, NULL);
}

Crack::~Crack(void)
{
	std::vector<int> pids;
	std::map<std::string, lauch_param>::iterator it;
	for(it = running.begin(); it != running.end(); it++)
		pids.push_back(it->second.pid);
	
	for(int i = 0; i < pids.size(); i++)
		kill(pids[i], SIGKILL);
	
	pthread_rwlock_destroy(&rwlock);
}

int Crack::SetPath(const char* _path)
{
	strncpy(path, _path, sizeof(path));
	return 0;
}

void Crack::SetToolName(const char* name)
{
	strncpy(toolname, name, sizeof(toolname));
}
	
int Crack::RunningTasks()const
{
	return running.size();
}

void Crack::RegisterCallback(ProcessDone done, ProgressStatus status)
{
	this->doneFunc = done;
	this->statusFunc = status;
}

int Crack::StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceIds[], int ndevices)
{
	int pid = this->Launcher(item, gpu, deviceIds, ndevices);
	if(pid <= 0) return ERR_LAUCH_TASK;

	return 0;
}

int Crack::Kill(const char* guid)
{
	WLock lock(&rwlock);
	
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
	{
		//非法的，没有该解密任务
		return ERR_NO_THISTASK;
	}

	int pid = it->second.pid;
	if(pid <= 0)
		return ERR_NO_THISTASK;
	
	int status = kill(pid, SIGKILL);
	if(status < 0)
	{
		//终止任务失败
		CLog::Log(LOG_LEVEL_WARNING, "Crack: Failed to kill [pid=%d, msg=%s]\n", pid, strerror(errno));
		return ERR_FAILED_KILL;
	}
	waitpid(pid, &status, 0);

	close(it->second.read_fd);
	close(it->second.write_fd);
	running.erase(it);
	CLog::Log(LOG_LEVEL_NOMAL, "Crack: Stop [guid=%s] OK\n", guid);
	return 0;
}
	

int Crack::StopCrack(const char* guid)
{
	return this->Kill(guid);
}

int Crack::Exec(const char* guid, const char* path, const char* params, void* (*monitor)(void*),
				bool dupStdin, bool dupStdout, bool dupStderr)
{
#if defined(WIN32) || defined(WIN64)
	return 0;
#else
	char *args[32];
	int n = 0;
	const char *split = " ";
	char *lasts = NULL;
	char params2[4096];
	strncpy(params2, params, 4096); 
	
	args[n++] = (char*)path;
	if((args[n++] = strtok_r(params2, split, &lasts)) == NULL)
        return -1;
    while((args[n] = strtok_r(NULL, split, &lasts)) != NULL)
		n++;
	args[n] = NULL;

	int pid;
	int fd1[2], fd2[2];

	if(pipe(fd1) < 0 ||	pipe(fd2) < 0){
		perror("pipe");
		return ERR_LAUCH_TASK;
	}

	if((pid = fork()) < 0){
		perror("fork");
		return ERR_LAUCH_TASK;
	} else if(pid == 0){
		CLog::Log(LOG_LEVEL_NOMAL, "exec: Try to launch task [guid=%s, pid=%d] %s\n", guid, getpid(), params);
		
		close(fd1[1]);
		close(fd2[0]);
		close(0);
		close(1);
		close(2);
		if(dupStdin)
			dup2(fd1[0], STDIN_FILENO);
		if(dupStdout)
			dup2(fd2[1], STDOUT_FILENO);
        if(dupStderr)
			dup2(fd2[1], STDERR_FILENO);
        close(fd1[0]);  
		close(fd2[1]);

		if(execv(path, args) < 0){
			perror("execl");
			_exit(ERR_LAUCH_TASK);
		}
		_exit(127);
	} else {
		close(fd1[0]);
		close(fd2[1]);   
		int flag = fcntl(fd2[0], F_GETFL, 0);
		fcntl(fd2[0], F_SETFL, flag|O_NONBLOCK);
		
		usleep(1000*10);
		int status = -1;
		int rv = waitpid(pid, &status, WNOHANG);
		if(rv > 0){
			CLog::Log(LOG_LEVEL_ERROR, "exec: Failed to start child process %s [%d]\n", guid, pid);
			close(fd1[1]);
			close(fd2[0]);   
			return ERR_LAUCH_TASK;    		
		}

		{
			WLock lock(&rwlock);
			struct lauch_param lp = {pid, (pthread_t)-1, fd2[0], fd1[1], 0, 0, 0, 0};
			running[guid] = lp;
			
			pthread_t tid;
			thread_param* p = (thread_param*)malloc(sizeof(*p));
			memcpy(p->guid, guid, sizeof(p->guid));
			p->crack = this;
			pthread_create(&tid, NULL, monitor, (void *)p);
			running[guid].tid = tid;
		}
		
		return pid;
	}
#endif
}

int Crack::ReadFromLancher(const char* guid, char* buf, int n)
{
	int pid = -1;
	int fd = -1;
	
	{
		RLock lock(&rwlock);
		std::map<std::string, lauch_param>::iterator it = running.find(guid);
		if(it == running.end())
		{
			//非法的，没有该解密任务
			return ERR_NO_THISTASK;
		}
		pid = it->second.pid;
		fd = it->second.read_fd;
	}

#if defined(WIN32) || defined(WIN64)
	return 0;
#else
	int status = -1;
	int rv = waitpid(pid, &status, WNOHANG);
	if(rv > 0){
		CleanUp(guid);
		return ERR_CHILDEXIT;    //子进程已经结束		
	}

	if (-1 == rv && EINTR != errno){
		CleanUp(guid);
		return 0;
	}
	
	//下面是200毫秒的等待直至有数据可读
	{
		fd_set read_fdset;        
		struct timeval timeout;             
	
		FD_ZERO(&read_fdset);        
		FD_SET(fd, &read_fdset);             
		timeout.tv_sec = 0;        
		timeout.tv_usec = 500*1000;             
		int ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout); 
		if (ret == 0)
			return ERR_TIMEOUT;
	}
	
	return read(fd, buf, n);

#endif
}

int Crack::WriteToLancher(const char* guid, const char* buf, int n)
{
	int pid = -1;
	int fd = -1;
	
	{
		RLock lock(&rwlock);
		std::map<std::string, lauch_param>::iterator it = running.find(guid);
		if(it == running.end())
		{
			//非法的，没有该解密任务
			return ERR_NO_THISTASK;
		}
		pid = it->second.pid;
		fd = it->second.write_fd;
	}
#if defined(WIN32) || defined(WIN64)
	return 0;
#else
	int status = -1;
	int rv = waitpid(pid, &status, WNOHANG);
	if(rv > 0){
		CleanUp(guid);
		return ERR_CHILDEXIT;    //子进程已经结束		
	}

	if (-1 == rv && EINTR != errno){
		CleanUp(guid);
		return 0;
	}

	return write(fd, buf, n);
#endif

}

int Crack::CleanUp(const char* guid)
{
	WLock lock(&rwlock);
	
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
		return ERR_NO_THISTASK;
	
	close(it->second.read_fd);
	close(it->second.write_fd);
	running.erase(it);
	return 0;
}

int Crack::UpdateStatus(const char* guid, int progress, float speed, unsigned int elapseTime, unsigned int remainTime)
{
	return 0;
}
