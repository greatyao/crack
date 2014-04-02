#ifndef __OCLHASHCAT_H__
#define __OCLHASHCAT_H__

#include "Crack.h"
#include <map>
#include <string>

using namespace std;

class oclHashcat : public Crack
{
public:
	oclHashcat(void);
	~oclHashcat(void);
	map<string,struct maphashtarget> MapTargetHash;
	static void *MonitorThread(void *p);

	virtual int Launcher(const crack_block* item, bool gpu, unsigned short deviceId);
	virtual int Launcher(const crack_block* item, bool gpu, unsigned short* deviceIds, int ndevices);
	
	virtual int SupportMultiTasks()const;	
};

#endif
