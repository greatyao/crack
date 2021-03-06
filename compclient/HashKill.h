﻿/* HashKill.h
 *
 * Hashkill tool for cracking
 * Copyright (C) 2014 TRIMPS
 *
 * Craeted By YAO Wei at  03/17/2014
 */
 
#ifndef __HASHKILL_H__
#define __HASHKILL_H__

#include "Crack.h"

class HashKill : public Crack
{
public:
	HashKill(void);
	~HashKill(void);
	
	static void *MonitorThread(void *p);

	virtual int Launcher(const crack_block* item, bool gpu, unsigned short* deviceIds, int ndevices);
	
	virtual int SupportMultiTasks()const;	
};

#endif