

#ifndef _SERVER_FUNC_H_
#define _SERVER_FUNC_H_


#define MAX_BUF_LEN 1024 

VOID ProcessClientData(LPVOID lpParameter);
	
INT SendServerData(LPVOID pClient, LPBYTE pData, UINT len);

INT process_recv_data(LPVOID pClient, LPBYTE pData, UINT len);


#endif

