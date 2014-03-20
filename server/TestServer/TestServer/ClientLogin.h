
#ifndef _CLIENT_LOGIN_H_

#define _CLIENT_LOGIN_H_

#include <vector>


struct SUPER_CTL_LOGIN{
	
	OSVERSIONINFOEX OsVerInfoEx;
	IN_ADDR IPAddress;
	char HostName[50];
	GUID guid;
	HANDLE socket;

};


struct NORMAL_CTL_LOGIN{

	OSVERSIONINFOEX OsVerInfoEx;
	IN_ADDR IPAddress;
	char HostName[50];
	GUID guid;
	HANDLE socket;

};


struct COMPUTING_LOGIN{

	OSVERSIONINFOEX OsVerInfoEx;
	IN_ADDR IPAddress;
	char HostName[50];
	GUID guid;
	UINT cpuThreads;
	UINT gpuThreads;
	HANDLE socket;
};


struct LOGIN_INFO {

	BYTE			bToken;			// = 1
	OSVERSIONINFOEX	OsVerInfoEx;	// �汾��Ϣ
	IN_ADDR			IPAddress;		// �洢32λ��IPv4�ĵ�ַ���ݽṹ
	char			HostName[50];	// ������
	GUID			guid;
	UINT			cpuThreads;
	UINT			gpuThreads;
	BYTE			clientType;		// ָ�������Ϊ�������ƽڵ㡢���ƽڵ㡢����ڵ�
	HANDLE 			socket;

};


typedef struct _CLIENT_INFO_{

	time_t	m_livetime;
	LOGIN_INFO	m_LoginInfo;
	HANDLE	m_ClientSock;
	
}ClientInfo;


typedef std::vector <ClientInfo *> ClientList;

typedef std::vector <ClientInfo *>::iterator ClientIter;

//ClientList g_ClientList;



#endif


