
#ifndef _SERVER_RESP_H_
#define _SERVER_RESP_H_

#define MAX_BUF_LEN 9000 

//²âÊÔº¯Êý
void DispatchThread(void* p);

int Read(int sck, unsigned char *cmd, short* status, void* data, int size);
int Write(int sck, unsigned char cmd, short status, const void* data, int size,bool bCompress);

#endif
