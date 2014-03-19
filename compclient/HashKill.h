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
};

#endif