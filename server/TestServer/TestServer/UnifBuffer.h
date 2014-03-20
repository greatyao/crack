#pragma once
#include <windows.h>

class CUnifBuffer
{
	BYTE *m_pData;
	UINT m_LengthData;
	BYTE *m_pPos;

public:
	/**************************************************************************
	* ���캯������������
	***************************************************************************/
	CUnifBuffer(void);
	CUnifBuffer(BYTE *pData,UINT Length);
	~CUnifBuffer(void);

	
	/**************************************************************************
	* ��ͨ���ܺ���
	***************************************************************************/
	//��ȡ����������
	UINT  GetBuffer(BYTE *pBuffer,UINT LengthBuffer);
	PBYTE GetBuffer(void);
	
	//��ȡ�����С(����������)
	UINT GetLength(void);
	UINT GetItemLength(UINT uID);
	
	UINT SetBuffer(BYTE *pBuffer,UINT LengthBuffer);
	UINT SetBuffer(CUnifBuffer *unif);


	//��ȡ��������С
	//���	
	void ResetBuffer(void);
	void Empty(void);

	//��д����
	UINT ReadItem(UINT uID,BYTE *pReadBuffer,UINT LengthReadBuffer);
	BOOL WriteItem(UINT uID,BYTE *pWriteBuffer,UINT LengthWriteBuffer);

	
	/**************************************************************************
	* ��ݹ��ܺ���
	***************************************************************************/
	//��������
	//BOOL  SetCommand(UINT uCmd);
	//��ȡ����
	//UINT  GetCmdLength(void);
	//��ȡ����
	//PBYTE GetCmdData(void);

protected:
	/**************************************************************************
	* �ڲ�����
	***************************************************************************/
	BYTE * _Alloc(SIZE_T dwBytes);
	BYTE   _Free(LPVOID lpMem);
};


class CCmdUnifBuffer : public CUnifBuffer
{	
	PBYTE m_pCmdData;
	UINT  m_uCmdLen;
public:
	CCmdUnifBuffer(void);
	~CCmdUnifBuffer(void);
public:
	BOOL UpdateTask(UINT uMinLen,UINT uMaxLen,char *sCharSet,CUnifBuffer *pUnif);//�ϴ�����

	UINT  GetCmdLength(void);
	PBYTE GetCmdData(void);
};