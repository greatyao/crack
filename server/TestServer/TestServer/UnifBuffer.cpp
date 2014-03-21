//#include "unif.h"
//#include "macros.h"
#include "stdafx.h"
#include "UnifBuffer.h"
//#include "RecvDone.h"

CUnifBuffer::CUnifBuffer(void)
{
	m_pData = NULL;
	m_LengthData = 0;
}

CUnifBuffer::CUnifBuffer(BYTE *pData,UINT Length)
{
	if( (pData)&&(Length) )
	{
		m_pData = _Alloc(Length);
		if(m_pData)
		{
			CopyMemory(m_pData,pData,Length);
			m_LengthData = Length;
		}
		else
		{
			m_LengthData = 0;
		}
	}
	else
	{
		m_pData = NULL;
		m_LengthData = 0;
	}
}

CUnifBuffer::~CUnifBuffer(void)
{
	if(m_pData)
	{
		ResetBuffer();
	}
}

//��ȡ����������
UINT CUnifBuffer::GetBuffer(BYTE *pBuffer,UINT LengthBuffer)
{
	if( (pBuffer==NULL)||(LengthBuffer==0) )
		return 0;

	UINT len = LengthBuffer;
	if(len>m_LengthData) 
		len=m_LengthData;

	CopyMemory(pBuffer,m_pData,len);
	return len;
}

BYTE *CUnifBuffer::GetBuffer(void)
{
	return m_pData;
}

UINT CUnifBuffer::SetBuffer(BYTE *pBuffer,UINT LengthBuffer)
{
	this->ResetBuffer();
	m_pData = _Alloc(LengthBuffer);
	if(!m_pData) return 0;
	m_LengthData = LengthBuffer;
	memcpy(m_pData,pBuffer,m_LengthData);
	return LengthBuffer;
}

UINT CUnifBuffer::SetBuffer(CUnifBuffer *unif)
{
	this->ResetBuffer();
	UINT uLen = unif->GetLength();
	m_pData = _Alloc(uLen);
	if(!m_pData) return 0;
	m_LengthData = uLen;
	unif->GetBuffer(m_pData,m_LengthData);
	return uLen;
}

//��ȡ��������С
void CUnifBuffer::ResetBuffer(void)
{
	if(m_pData)
	{
		_Free(m_pData);
		m_pData = NULL;
		m_LengthData = 0;
	}
}


UINT CUnifBuffer::GetLength(void)
{
	return m_LengthData;
}

//��д����
UINT CUnifBuffer::GetItemLength(UINT uID)
{
	UINT uPos = 0;	//��ǰ���ݶ�ȡ��λ��
	UINT uValueID = 0;//��ȡ����ֵ
	UINT uValueLen = 0;

	m_pPos =  NULL;

	while( uPos<m_LengthData )
	{
		uValueLen = 0;

		if( (uPos+8)>m_LengthData ) 
			break;

		//��ȡ����
		CopyMemory(&uValueID,&m_pData[uPos],4);//read id
		uPos += 4;
		CopyMemory(&uValueLen,&m_pData[uPos],4);//read len
		uPos += 4;

		if(uValueID!=uID) //����
		{
			uPos += uValueLen;
			continue;
		}

		if( (uPos+uValueLen)>m_LengthData)
		{
			uValueLen = 0;
		}
		
		m_pPos = &m_pData[uPos];//ָ������
		break;
	}
	return uValueLen;
}

UINT CUnifBuffer::ReadItem(UINT uID,BYTE *pReadBuffer,UINT LengthReadBuffer)
{
	UINT lenItem = GetItemLength(uID);
	if( (lenItem)&&(m_pPos) )
	{
		if( LengthReadBuffer<lenItem )
		{
			CopyMemory(pReadBuffer,m_pPos,LengthReadBuffer);
			return LengthReadBuffer;
		}
		else
		{
			CopyMemory(pReadBuffer,m_pPos,lenItem);
			return lenItem;
		}				
	}
	return 0;
}

BOOL CUnifBuffer::WriteItem(UINT uID,BYTE *pWriteBuffer,UINT LengthWriteBuffer)
{
	UINT lenItem = GetItemLength(uID);
	if( (lenItem)&&(m_pPos) )//���ݶ��Ѿ�����
	{
		lenItem+=8;//�������ݵĳ���
		m_pPos = m_pPos -8;//ָ����ڵ�����
		//|__________1____________|_______2_______|_____3_____|
		BYTE *pCopyData = _Alloc(m_LengthData-lenItem);
		MoveMemory(pCopyData,m_pData,m_pPos-m_pData);
		m_pPos = m_pPos+lenItem;
		MoveMemory(&pCopyData[m_pData-m_pPos],m_pPos,m_LengthData-(m_pPos-m_pData));
		
		_Free(m_pData);
		m_pData = pCopyData;
	}
		
	//׷������
	BYTE *pNewData = _Alloc(m_LengthData+8+LengthWriteBuffer);
	if(pNewData==NULL) return 0;

	CopyMemory(pNewData,m_pData,m_LengthData);//��������
		
	CopyMemory(&pNewData[m_LengthData],&uID,4);
	m_LengthData+=4;
	CopyMemory(&pNewData[m_LengthData],&LengthWriteBuffer,4);
	m_LengthData+=4;
	CopyMemory(&pNewData[m_LengthData],pWriteBuffer,LengthWriteBuffer);
	m_LengthData+=LengthWriteBuffer;

	_Free(m_pData);
	m_pData = pNewData;
	return TRUE;
}

BYTE * CUnifBuffer::_Alloc(SIZE_T dwBytes)
{
	return (BYTE*)HeapAlloc( GetProcessHeap(),HEAP_ZERO_MEMORY,dwBytes);
}

BYTE   CUnifBuffer::_Free(LPVOID lpMem)
{
	if(lpMem)
	{
		return HeapFree( GetProcessHeap(),HEAP_NO_SERIALIZE,lpMem);
	}
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
//  �����װ
//
/////////////////////////////////////////////////////////////////////////////////////////////
CCmdUnifBuffer::CCmdUnifBuffer(void)
{
	//CUnifBuffer::CUnifBuffer();	
	m_pCmdData = NULL;
	m_uCmdLen  = 0;
}
CCmdUnifBuffer::~CCmdUnifBuffer(void)
{
	if(m_pCmdData)
	{
		CUnifBuffer::_Free(m_pCmdData);
		m_pCmdData = NULL;
		m_uCmdLen = 0;
	}
	//CUnifBuffer::~CUnifBuffer();
}
UINT  CCmdUnifBuffer::GetCmdLength(void)
{
	return m_uCmdLen;
}
PBYTE CCmdUnifBuffer::GetCmdData(void)
{
	return m_pCmdData;
}

BOOL CCmdUnifBuffer::UpdateTask(UINT uMinLen,UINT uMaxLen,char *sCharSet,CUnifBuffer *pUnif)//�ϴ�����
{
	//�жϲ���
	if( (uMinLen<1)||(uMaxLen<uMinLen)||(sCharSet==NULL)||(pUnif==NULL) ) return FALSE;

	//�жϳ���
	int lenCharset = lstrlenA(sCharSet);
	if(lenCharset<1) return FALSE;

	//make cmd data
	CUnifBuffer::ResetBuffer();
	if(m_pCmdData)
	{
		CUnifBuffer::_Free(m_pCmdData);
		m_pCmdData = 0;
		m_uCmdLen  = 0;
	}

	//BUFFER;
//	CUnifBuffer::WriteItem(PWD_MIN_LENGTH,(BYTE*)&uMinLen,sizeof(UINT));
//	CUnifBuffer::WriteItem(PWD_MAX_LENGTH,(BYTE*)&uMaxLen,sizeof(UINT));
//	CUnifBuffer::WriteItem(PWD_CHAR_SET,  (BYTE*)sCharSet,lenCharset);
//	CUnifBuffer::WriteItem(UNIF_BUFFER,   pUnif->GetBuffer(),pUnif->GetLength());
	
	//
	m_uCmdLen = CUnifBuffer::GetLength()+1;
	m_pCmdData = CUnifBuffer::_Alloc(m_uCmdLen);
	if(m_pCmdData==0) return FALSE;

	//m_pCmdData[0]=CMD_UPLOAD_TASK;
//	m_pCmdData[0] = CMD_TASK_UPLOAD;
	memcpy( &m_pCmdData[1],CUnifBuffer::GetBuffer(),m_uCmdLen-1);
	return TRUE;
}

/*
BOOL CCmdUnifBuffer::ParserTask(PBYTE pData,UINT lenData,WorkItem *wItem)
{
	if( (pData==0)||(lenData==0)||(wItem==0)) return FALSE;

	typedef struct _WORK_ITEM
	{
		GUID m_Guid;

		CUnifBuffer m_CiphertextInfo;
		CString m_BeginText;
		CString m_EndText;
		CString m_CharacterSet;

		UINT m_ItemStatus;
		//�и��Ľ���������
	} WorkItem;



	return TRUE;
}
*/