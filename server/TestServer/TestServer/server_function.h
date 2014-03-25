

#ifndef _SERVER_FUNC_H_
#define _SERVER_FUNC_H_


#define MAX_BUF_LEN 1024 


VOID ProcessClientData1(LPVOID lpParameter);


VOID ProcessClientData(LPVOID lpParameter);
	
INT SendServerData(LPVOID pClient, LPBYTE pData, UINT len);

INT SendServerData2(LPVOID pClient, LPBYTE pData, UINT len);

INT SimpleSendData(LPVOID pClient, LPBYTE pData, UINT len,int tag);


INT process_recv_data(LPVOID pClient, LPBYTE pData, UINT len);


#endif

