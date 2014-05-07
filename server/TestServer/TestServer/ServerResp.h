
#ifndef _SERVER_RESP_H_
#define _SERVER_RESP_H_

#define MAX_BUF_LEN 9000 

#include <BaseTsd.h>

//²âÊÔº¯Êý
void DispatchThread(void* p);

int Read(UINT_PTR sck, unsigned char *cmd, short* status, void* data, int size);
int Write(UINT_PTR sck, unsigned char cmd, short status, const void* data, int size,bool bCompress);

#endif
