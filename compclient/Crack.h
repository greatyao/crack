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

typedef int (*ProcessDone)(char* guid, bool cracked, const char* passwd);
typedef int (*ProgressStatus)(char* guid, int progress, float speed, unsigned int remainTime);

class Crack
{
protected:
	Crack(void);
	virtual ~Crack(void);
	int Exec(const char* guid, const char* path, const char* params, void* (*monitor)(void*));
	int Kill(const char* guid);
	int CleanUp(const char* guid);
	int UpdateStatus(const char* guid, int progress, float speed, unsigned int elapseTime, unsigned int remainTime);
	
	std::map<std::string, struct lauch_param> running;
	ProcessDone doneFunc;
	ProgressStatus statusFunc;

public:
	int StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceId);
	int StopCrack(const char* guid);
	
	//ע��ص�����
	void RegisterCallback(ProcessDone done, ProgressStatus status);
	
	//��ȡ���ý��̵�stdout��stderr�������Ϣ
	//������, -1��ʾû�����ݣ� 0��ʾ�����Ѿ�������������ʾʵ�ʶ����������
	int ReadFromLancher(const char* guid, char* buf, int n);
	
	//�����ý�������д��Ϣ��ģ����ý��̵�stdin
	//������, 0��ʾ�����Ѿ�������������ʾʵ��д���������
	int WriteToLancher(const char* guid, const char* buf, int n);

	//���µ��麯����Ҫ����ÿ���������ʵ��
	
	//����
	virtual int Launcher(const crack_block* item, bool gpu, unsigned short deviceId) = 0;
};

#endif
