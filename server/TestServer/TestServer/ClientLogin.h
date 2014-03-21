
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
	OSVERSIONINFOEX	OsVerInfoEx;	// 版本信息
	IN_ADDR			IPAddress;		// 存储32位的IPv4的地址数据结构
	char			HostName[50];	// 主机名
	GUID			guid;
	UINT			cpuThreads;
	UINT			gpuThreads;
	BYTE			clientType;		// 指定计算机为超级控制节点、控制节点、计算节点
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


