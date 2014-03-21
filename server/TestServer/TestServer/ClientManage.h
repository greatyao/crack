#pragma once

#include "ClientLogin.h"
#include "PacketProcess.h"

class CClientManage
{

private:
	CRITICAL_SECTION m_CriticalSection;
	ClientList m_ClientList;
	UINT	m_nCurClient;

public:
	CClientManage(void);
	~CClientManage(void);
	
	VOID Lock();
	VOID UnLock();

	INT InsertClient(ClientInfo *pClientInfo);
	INT UpdateKeepliveInfo(GUID guid,time_t t);
	INT UpdateClient(GUID guid,ClientInfo *pClientInfo);
	INT RemoveClient(GUID guid);
	
	INT GetCompCount();
	INT GetSupCount();
	INT GetCtlCount();

	LPVOID GetCompNodes(UINT *size);
	LPVOID GetSupNodes(UINT *size);
	LPVOID GetCtlNodes(UINT *size);


private:
	BOOL isSupNode(BYTE roleType);
	BOOL isCtlNode(BYTE roleType);
	BOOL isComNode(BYTE roleType);

	LPVOID _Alloc(UINT size);
	VOID _Free(LPVOID pt);

};

