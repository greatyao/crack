#ifndef __HASHKILL_H__
#define __HASHKILL_H__

#include "Crack.h"

class HashKill : public Crack
{
public:
	HashKill(void);
	~HashKill(void);
	
	static void *MonitorThread(void *p);

	virtual int Launcher(const crack_block* item, bool gpu, unsigned short deviceId);
	
	//��ֹ
	virtual int Kill(const char* guid);
	
	//��ȡ����
	virtual int ObtainProgress(const char* guid, float* progress, float* speed, float* time);
};

#endif