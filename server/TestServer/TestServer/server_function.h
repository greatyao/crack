

#ifndef _SERVER_FUNC_H_
#define _SERVER_FUNC_H_


//#define MAX_BUF_LEN 1024 
#define MAX_BUF_LEN 9000 

//���Ժ���
VOID ProcessClientData1(LPVOID lpParameter);

INT SendServerData2(LPVOID pClient, LPBYTE pData, UINT len);

INT SimpleSendData(LPVOID pClient, LPBYTE pData, UINT len,int tag);


//ʵ�ʹ���

VOID ProcessClientData(LPVOID lpParameter);

INT SendServerData(LPVOID pClient, LPBYTE pData, UINT len);

INT process_recv_data(LPVOID pClient, LPBYTE pData, UINT len,BYTE cmdTag);









#endif

