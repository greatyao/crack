
#ifndef _SERVER_RESP_H_
#define _SERVER_RESP_H_



int SendDataToPeer(void *pclient, unsigned char * pdata, unsigned int len);

int RecvDataFromPeer(void *pclient, unsigned char * pdata, unsigned int len);

int Read(int sck, unsigned char *cmd, short* status, void* data, int size);
int Write(int sck, unsigned char cmd, const void* data, int size);

#endif
