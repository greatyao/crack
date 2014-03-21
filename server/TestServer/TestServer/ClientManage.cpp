#include "StdAfx.h"
#include "ClientManage.h"

CClientManage::CClientManage(void)
{
	InitializeCriticalSection(&m_CriticalSection);

	m_nCurClient = 0;
}

CClientManage::~CClientManage(void)
{
	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	if (m_ClientList.size() > 0 ){

		
		for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
			pClientInfo = *iter;
			delete pClientInfo;   //删除客户端信息内存

		}

		m_ClientList.clear();  //清楚客户端列表
		m_nCurClient = 0;
	}
	
	DeleteCriticalSection(&m_CriticalSection);

}

INT CClientManage::InsertClient(ClientInfo * pClientInfo)
{
	
	int ret = 0;

	if (!pClientInfo){
	
		Lock();
		m_ClientList.push_back(pClientInfo);
		m_nCurClient +=1;
		UnLock();
		ret = 0;
	}else {

		ret = -1;
	}

	return ret;
}


VOID CClientManage::Lock(){

	EnterCriticalSection(&m_CriticalSection);
}

VOID CClientManage::UnLock(){

	LeaveCriticalSection(&m_CriticalSection);

}


INT CClientManage::UpdateKeepliveInfo(GUID guid,time_t t){

	
	int ret = 0;
	Lock();
	ClientInfo *pTemp = NULL;
	ClientIter iter;

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pTemp = *iter;
		if (memcmp(&(pTemp->m_LoginInfo.guid),&guid,sizeof(GUID)) == 0){
			pTemp->m_livetime = t;
			break;
		}
	}
	UnLock();
	return ret;
}

INT CClientManage::UpdateClient(GUID guid,ClientInfo *pClientInfo){

	int ret = 0;
	Lock();
	ClientInfo *pTemp = NULL;

	ClientIter iter;

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pTemp = *iter;
		if (memcmp(&(pTemp->m_LoginInfo.guid),&guid,sizeof(GUID)) == 0){
			
			memcpy(pTemp,pClientInfo,sizeof(ClientInfo));
			break;
		}

	}
	
	UnLock();
	return ret;
}


INT CClientManage::RemoveClient(GUID guid){

	Lock();
	
	ClientInfo *pClientInfo = NULL;

	ClientIter iter;

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (memcmp(&(pClientInfo->m_LoginInfo.guid),&guid,sizeof(GUID)) == 0){

			break;
		}

	}
	
	if (!pClientInfo){

		m_ClientList.erase(iter);
		m_nCurClient -=1;

	}

	delete pClientInfo;

	UnLock();
	
	return 0;
}


INT CClientManage::GetCompCount(){


	int count = 0;
	Lock();

	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (isComNode(pClientInfo->m_LoginInfo.clientType)){

			count+=1;
		}
		
	}
	
	UnLock();
	return count;
}

INT CClientManage::GetSupCount(){

	int count = 0;
	Lock();
	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (isSupNode(pClientInfo->m_LoginInfo.clientType)){

			count+=1;
		}
		
	}
	UnLock();

	return count;
}

INT CClientManage::GetCtlCount(){

	int count = 0;
	Lock();
	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (isCtlNode(pClientInfo->m_LoginInfo.clientType)){

			count+=1;
		}
		
	}
	
	UnLock();
	return count;
}

LPVOID CClientManage::GetCompNodes(UINT *size){
	
	Lock();
	int count = 0;
	int i= 0;
	ClientInfo *pclients = NULL;
	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	//获得计算节点数目
	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = * iter;
		if (isComNode(pClientInfo->m_LoginInfo.clientType)){

			count+=1;
		}
	}
	

	pclients = (ClientInfo *)_Alloc(count*sizeof(ClientInfo));
	if (!pclients){

		UnLock();
		*size = 0;
		return NULL;
	}

	//获得计算节点信息
	i = 0;
	int clientLen = sizeof(ClientInfo);

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = * iter;
		if (isComNode(pClientInfo->m_LoginInfo.clientType)){

			CopyMemory(&pclients[i++],pClientInfo,clientLen);
		}
	}

	*size = i;
	UnLock();
	return pclients;
}
LPVOID CClientManage::GetSupNodes(UINT *size){

	Lock();
	int count = 0;
	int i= 0;
	ClientInfo *pclients = NULL;
	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	//获得超级节点数目
	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = * iter;
		if (isSupNode(pClientInfo->m_LoginInfo.clientType)){

			count+=1;
		}
	}
	

	pclients = (ClientInfo *)_Alloc(count*sizeof(ClientInfo));
	if (!pclients){

		UnLock();
		*size = 0;
		return NULL;
	}

	//获得超级节点信息
	i = 0;
	int clientLen = sizeof(ClientInfo);

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (isSupNode(pClientInfo->m_LoginInfo.clientType)){

			CopyMemory(&pclients[i++],pClientInfo,clientLen);
		}
	}

	*size = i;
	UnLock();
	return pclients;
}
LPVOID CClientManage::GetCtlNodes(UINT *size){

	Lock();
	int count = 0;
	int i= 0;
	ClientInfo *pclients = NULL;
	ClientInfo *pClientInfo = NULL;
	ClientIter iter;

	//获得控制节点数目
	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (isCtlNode(pClientInfo->m_LoginInfo.clientType)){

			count+=1;
		}
	}
	

	pclients =(ClientInfo *)_Alloc(count*sizeof(ClientInfo));
	if (!pclients){

		UnLock();
		*size = 0;
		return NULL;
	}

	//获得控制节点信息
	i = 0;
	int clientLen = sizeof(ClientInfo);

	for(iter = m_ClientList.begin();iter != m_ClientList.end();iter++){
		
		pClientInfo = *iter;
		if (isCtlNode(pClientInfo->m_LoginInfo.clientType)){

			CopyMemory(&pclients[i++],pClientInfo,clientLen);
		}
	}

	UnLock();

	*size = i;
	return pclients;
}

BOOL CClientManage::isSupNode(BYTE roleType){

	if (roleType == ROLE_SUPER_CONTROL)
		return TRUE;
	return FALSE;
}


BOOL CClientManage::isCtlNode(BYTE roleType){

	if (roleType == ROLE_NORMAL_CONTROL)
		return TRUE;

	return FALSE;
}


BOOL CClientManage::isComNode(BYTE roleType){

	if (roleType == ROLE_COMPUTE)
		return TRUE;

	return FALSE;
}


LPVOID CClientManage::_Alloc(UINT size){

	
	LPBYTE p = NULL;

	p = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);
	
	return (LPVOID)p;

}

VOID CClientManage::_Free(LPVOID pt){

	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pt);

}



