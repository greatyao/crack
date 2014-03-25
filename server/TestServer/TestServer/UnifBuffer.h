#pragma once
#include <windows.h>

class CUnifBuffer
{
	BYTE *m_pData;
	UINT m_LengthData;
	BYTE *m_pPos;

public:
	/**************************************************************************
	* 构造函数和析构函数
	***************************************************************************/
	CUnifBuffer(void);
	CUnifBuffer(BYTE *pData,UINT Length);
	~CUnifBuffer(void);

	
	/**************************************************************************
	* 普通功能函数
	***************************************************************************/
	//获取缓冲区数据
	UINT  GetBuffer(BYTE *pBuffer,UINT LengthBuffer);
	PBYTE GetBuffer(void);
	
	//获取缓冲大小(不含命令字)
	UINT GetLength(void);
	UINT GetItemLength(UINT uID);
	
	UINT SetBuffer(BYTE *pBuffer,UINT LengthBuffer);
	UINT SetBuffer(CUnifBuffer *unif);


	//获取缓冲区大小
	//清空	
	void ResetBuffer(void);
	void Empty(void);

	//读写数据
	UINT ReadItem(UINT uID,BYTE *pReadBuffer,UINT LengthReadBuffer);
	BOOL WriteItem(UINT uID,BYTE *pWriteBuffer,UINT LengthWriteBuffer);

	
	/**************************************************************************
	* 快捷功能函数
	***************************************************************************/
	//设置命令
	//BOOL  SetCommand(UINT uCmd);
	//获取长度
	//UINT  GetCmdLength(void);
	//获取数据
	//PBYTE GetCmdData(void);

protected:
	/**************************************************************************
	* 内部函数
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
	BOOL UpdateTask(UINT uMinLen,UINT uMaxLen,char *sCharSet,CUnifBuffer *pUnif);//上传任务

	UINT  GetCmdLength(void);
	PBYTE GetCmdData(void);
};