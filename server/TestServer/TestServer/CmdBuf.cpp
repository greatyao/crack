
#include "StdAfx.h"
#include "CmdBuf.h"


CCmdBuf::CCmdBuf(void){


}

CCmdBuf::CCmdBuf(BYTE cmd,CUnifBuffer unifbuf)
{
	m_nCmdLen = unifbuf.GetItemLength(cmd);
	if (m_nCmdLen < 0 ) {

		m_pCmdData = NULL;
		m_nCmdLen = 0;
		return ;
	}

	m_pCmdData = (LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,m_nCmdLen);
	if (!m_pCmdData){
		
		m_pCmdData = NULL;
		m_nCmdLen = 0;
		return ;

	}

	unifbuf.ReadItem(cmd,m_pCmdData,m_nCmdLen);

}

CCmdBuf::~CCmdBuf(void)
{
	m_nCmdLen = 0;
	if (m_pCmdData)
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,m_pCmdData);
	m_pCmdData = NULL;

}


UINT CCmdBuf::GetCmdLen(void){

	return m_nCmdLen;
}

LPBYTE CCmdBuf::GetCmdData(void){

	return m_pCmdData;
}

