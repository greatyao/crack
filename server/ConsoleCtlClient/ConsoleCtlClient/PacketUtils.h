
#ifndef _PACKET_UTILS_H_
#define _PACKET_UTILS_H_

#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "algorithm_types.h"
#include "ReqPacket.h"

#include "CLog.h"

#define MAX_BUF_LEN 1024

#define CTL_HDR_LEN sizeof(control_header)


/*
printf("0 Login Server\n");
	printf("1 HeartBeat Server\n");
	printf("2 Upload Task\n");
	printf("3 Start Task,include split task to workitems\n");

	printf("4 Stop Task\n");
	printf("5 Delete Task\n");
	printf("6 Pause Task\n");

	printf("7 Get A Task Result\n");
	
	printf("8 Get Tasks Status\n");
	printf("9 Get Computing Clients Status\n");
	printf("10 exit\n");

	*/
int genLoginPacket(unsigned char *pdata, int *len);

int genKeeplivePacket(unsigned char *pdata, int *len);


int genUploadTaskPacket(unsigned char *pdata, int *len);

int genStartTaskPacket(unsigned char *pdata, int *len);



int genStopTaskPacket(unsigned char *pdata, int *len);

int genDeleteTaskPacket(unsigned char *pdata, int *len);



int genPauseTaskPacket(unsigned char *pdata, int *len);

int genGetTaskResPacket(unsigned char *pdata, int *len);



int genTaskStatusPacket(unsigned char *pdata, int *len);

int genClientListPacket(unsigned char *pdata, int *len);


#endif


