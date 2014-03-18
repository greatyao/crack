#include "Crack.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(WIN64)
#else
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#endif

Crack::Crack(void)
{
}

Crack::~Crack(void)
{
}

int Crack::StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceId)
{
	//首先需要验证数据的有效性


	int pid = this->Launcher(item, gpu, deviceId);
	if(pid <= 0) return ERR_LAUCH_TASK;

	return 0;
}

int Crack::StopCrack(const char* guid)
{
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
	{
		//非法的，没有该解密任务
		return ERR_NO_THISTASK;
	}

	int status = this->Kill(guid);
	if(status < 0)
	{
		//终止任务失败
		return ERR_FAILED_KILL;
	}

	running.erase(it);
	return 0;
}

int Crack::Exec(const char* guid, const char* path, const char* params, void* (*monitor)(void*))
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
		close(fd1[1]);
		close(fd2[0]);
		close(0);close(1);close(2);
		dup2(fd1[0], STDIN_FILENO);
		dup2(fd2[1], STDOUT_FILENO);
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
		
		usleep(1000*300);
		int status = -1;
		int rv = waitpid(pid, &status, WNOHANG);
		if(rv > 0){
			CLog::Log(LOG_LEVEL_ERROR, "exec: failed to start child process\n");
			close(fd1[1]);
			close(fd2[0]);   
			return ERR_LAUCH_TASK;    		
		}

		pthread_t tid;
		{
			thread_param* p = (thread_param*)malloc(sizeof(*p));
			memcpy(p->guid, guid, sizeof(p->guid));
			p->crack = this;
			pthread_create(&tid, NULL, monitor, (void *)p);
		}
		
		struct lauch_param p = {pid, tid, fd2[0], fd1[1], 0, 0, 0, 0};
		running[guid] = p;
		
		return pid;
	}
#endif
}

int Crack::ReadFromLancher(const char* guid, char* buf, int n)
{
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
	{
		//非法的，没有该解密任务
		return ERR_NO_THISTASK;
	}
	int pid = it->second.pid;

#if defined(WIN32) || defined(WIN64)
	return 0;
#else
	int status = -1;
	int rv = waitpid(pid, &status, WNOHANG);
	if(rv > 0){
		CleanUp(guid);
		return 0;    //子进程已经结束		
	}

	if (-1 == rv && EINTR != errno){
		CleanUp(guid);
		return 0;
	}

	return read(it->second.read_fd, buf, n);

#endif
}

int Crack::WriteToLancher(const char* guid, const char* buf, int n)
{
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
	{
		//非法的，没有该解密任务
		return ERR_NO_THISTASK;
	}
	int pid = it->second.pid;
#if defined(WIN32) || defined(WIN64)
	return 0;
#else
	int status = -1;
	int rv = waitpid(pid, &status, WNOHANG);
	if(rv > 0){
		CleanUp(guid);
		return 0;    //子进程已经结束		
	}

	if (-1 == rv && EINTR != errno){
		CleanUp(guid);
		return 0;
	}

	return write(it->second.write_fd, buf, n);
#endif

}

int Crack::CleanUp(const char* guid)
{
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
		return ERR_NO_THISTASK;
	
	close(it->second.read_fd);
	close(it->second.write_fd);
	running.erase(it);
	return 0;
}

int Crack::UpdateStatus(const char* guid, float progress, float speed, float elapseTime, float remainTime)
{
	std::map<std::string, lauch_param>::iterator it = running.find(guid);
	if(it == running.end())
		return ERR_NO_THISTASK;
		
	it->second.progress = progress;
	it->second.speed = speed;
	it->second.retain_time = remainTime;
	it->second.cost_time = elapseTime;
	return 0;
}
