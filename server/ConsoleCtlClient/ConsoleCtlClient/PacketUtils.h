
#ifndef _PACKET_UTILS_H_
#define _PACKET_UTILS_H_

#include <stdio.h>
#include <string.h>

#include <winsock.h>
#include "macros.h"
#include "algorithm_types.h"
#include "ReqPacket.h"

#include "CLog.h"
//#include "global.h"

//#define MAX_BUF_LEN 1024

#define MAX_BUF_LEN 9000


#define CTL_HDR_LEN sizeof(control_header)



/*



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


int genTaskFileUpload(unsigned char *pdata, int *len);


int genTaskFileUploadStart(unsigned char *pdata,int *len);


int genTaskFileUploading(unsigned char *pdata, int *len);


#endif


