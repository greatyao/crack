

#ifndef _RES_PACKET_PARSE_H_
#define _RES_PACKET_PARSE_H_

#include <stdio.h>
#include <string.h>
#include <winsock.h>

#include "macros.h"
#include "algorithm_types.h"
#include "ResPacket.h"

#include "CLog.h"

//#include "global.h"


#define RES_BUF_LEN 1024

void getLoginResPacket(unsigned char *pdata, int len);

void getKeepliveResPacket(unsigned char *pdata, int len);


void getUploadTaskResPacket(unsigned char *pdata, int len);

void getStartTaskResPacket(unsigned char *pdata, int len);



void getStopTaskResPacket(unsigned char *pdata, int len);

void getDeleteTaskResPacket(unsigned char *pdata, int len);



void getPauseTaskResPacket(unsigned char *pdata, int len);

void getGetTaskResPacket(unsigned char *pdata, int len);



void getTaskStatusResPacket(unsigned char *pdata, int len);

void getClientListResPacket(unsigned char *pdata, int len);


//get server file upload start 
void getFileUploadRes(unsigned char *pdata , int len);

void getFileUploadStartRes(unsigned char *pdata, int len);

void getFileUploadEndRes(unsigned char *pdata, int len);


#endif



