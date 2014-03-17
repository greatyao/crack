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
	float progress;
	float speed;
	float retain_time;
};

struct thread_param
{
	Crack* crack;
	char guid[40];
};

class Crack
{
protected:
	Crack(void);
	virtual ~Crack(void);
	int Exec(const char* guid, const char* path, const char* params, void* (*monitor)(void*));
	int CleanUp(const char* guid);
	int UpdateStatus(const char* guid, float progress, float speed, float remainTime);
	
	std::map<std::string, struct lauch_param> running;

public:
	int StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceId);
	int StopCrack(const char* guid);
	
	//��ȡ���ý��̵�stdout��stderr�������Ϣ
	//������, -1��ʾû�����ݣ� 0��ʾ�����Ѿ�������������ʾʵ�ʶ����������
	int ReadFromLancher(const char* guid, char* buf, int n);
	
	//�����ý�������д��Ϣ��ģ����ý��̵�stdin
	//������, 0��ʾ�����Ѿ�������������ʾʵ��д���������
	int WriteToLancher(const char* guid, const char* buf, int n);

	//���µ��麯����Ҫ����ÿ���������ʵ��
	
	//����
	virtual int Launcher(const crack_block* item, bool gpu, unsigned short deviceId) = 0;
	
	//��ֹ
	virtual int Kill(const char* guid) = 0;
	
	//��ȡ����
	virtual int ObtainProgress(const char* guid, float* progress, float* speed, float* time) = 0;
};

#endif
