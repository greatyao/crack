
#include "stdafx.h"

#include "CLog.h"
#pragma warning (disable:4995)
#include <Strsafe.h>

BOOL   CLog::m_bInited = FALSE;
BOOL   CLog::m_bLogDate = FALSE;
UINT   CLog::m_uLogType = LOG_TO_SCREEN;
HANDLE CLog::m_hOutputConsole = NULL;
HANDLE CLog::m_hOutputFile = NULL;

CRITICAL_SECTION CLog::m_csCriticalSection={0};
/*****************************************************************************/
// ͬ�����������ƣ��ڲ����ã�
/*****************************************************************************/
//��
void CLog::Lock(void)
{
	EnterCriticalSection(&m_csCriticalSection);
}
//����
void CLog::UnLock(void)
{
	LeaveCriticalSection(&m_csCriticalSection);
}
//������
BOOL CLog::TryLock(void)
{
	return TryEnterCriticalSection(&m_csCriticalSection);
}

/*****************************************************************************/
// ��ʼ����Ļ�������̨(�ڲ�����)
/*****************************************************************************/
BOOL CLog::InitConsole(void)
{
	if(m_hOutputConsole) 
		return TRUE;

	AllocConsole();
	/*
	if(AllocConsole()==FALSE)
	{
		Error("�������󣬷������̨ʧ�ܣ�������� %d\n",GetLastError());
		return FALSE;
	}*/

	/*
	m_hOutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(m_hOutputConsole) 
	{
		AttachConsole(-1);
		return TRUE;
	}*/

	m_hOutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(m_hOutputConsole) 
	{
		return TRUE;
	}
	return FALSE;
}


/*****************************************************************************/
// ��ʼ���ļ���־
/*****************************************************************************/
BOOL CLog::InitFileSystem(const char *cFile)
{
	m_hOutputFile = CreateFileA (cFile, 
			GENERIC_READ|GENERIC_WRITE, 
			FILE_SHARE_WRITE|FILE_SHARE_READ, 
			NULL, OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if( m_hOutputFile==INVALID_HANDLE_VALUE )
	{
		Error("������־�ļ�(%s)ʧ��,������� %d\n",cFile,GetLastError());
		return FALSE;
	}
	return TRUE;
}
	
//��ӡ�������(�ڲ�����)
int CLog::Printf(UINT uLevel,const char * buffer)
{
	DWORD cbWritten;

	if(m_uLogType==LOG_TO_FILE)
	{
		cbWritten = Write(uLevel,buffer);
	}
	else{
		//�������̨����	
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		WORD saved_attributes;

		//��ȡԭ������
		GetConsoleScreenBufferInfo(m_hOutputConsole, &consoleInfo);
		saved_attributes = consoleInfo.wAttributes;

		//������ɫ
		WORD wColor = 7;
		if( LOG_LEVEL_WARNING== uLevel)
			wColor = 11;
		if( LOG_LEVEL_ERROR== uLevel)
			wColor = 12;
		SetConsoleTextAttribute(m_hOutputConsole, wColor);
		//���		
		WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), buffer, lstrlenA(buffer),&cbWritten, 0 );
		//�ָ�
		SetConsoleTextAttribute(m_hOutputConsole, saved_attributes);
	}
	return cbWritten;
}
//д��־�ļ����ڲ����ã�
int CLog::Write(UINT uLevel,const char * buffer)
{
	DWORD dwWritten;
	SetFilePointer(m_hOutputFile, 0, 0, FILE_END);
	WriteFile(m_hOutputFile, (LPCVOID)buffer, lstrlenA(buffer), &dwWritten, NULL);
	return dwWritten;
}

//�ڲ���־�����debug������Ϣ�����
int CLog::Error(LPCSTR pszFormat, ... )
{
	const int len_buffer = 1024 * 1;//1k ����
	char	buffer[len_buffer];	

	va_list arglist;
	va_start (arglist, pszFormat);
	ZeroMemory(buffer,len_buffer);
	StringCchVPrintfA(buffer,len_buffer,pszFormat,arglist);
	va_end (arglist);

	OutputDebugStringA( buffer );
	return 0;
}
	
//va_list ��ʽ��
char * CLog::vaFormat(LPCSTR pszFormat,va_list arglist)
{
	const int len_buffer = 1024 * 8;//8k ����
	static char	buffer[len_buffer];	

	ZeroMemory(buffer,len_buffer);
	StringCchVPrintfA(buffer,len_buffer,pszFormat,arglist);
	return buffer;
}
/*****************************************************************************/
// ��ʼ����־ϵͳ
// uType : ָ����־���� 
//           LOG_TO_FILE   ��ʾ����־д���ļ�
//           LOG_TO_SCREEN ��ʾ����־��ӡ����Ļ
// bDate : ָ����־�Ƿ��ӡ��־ʱ��(������)
// cFile : ָ�������־���ļ��������� uType Ϊ LOG_TO_FILE��Ч
/*****************************************************************************/
BOOL CLog::InitLogSystem(UINT uType ,BOOL bDate ,const char *cFile )
{
	if(m_bInited)
	{	//�Ѿ���ʼ����,����趨Ϊ���Ը��ĳ�ʼ����Ϣ����Ҫ�����Ϣ�����³�ʼ��
		return TRUE;
	}

	m_uLogType = uType;
	m_bLogDate = bDate;

	if(m_uLogType==LOG_TO_FILE)
	{
		if(InitFileSystem(cFile)==FALSE)
		return FALSE;
	}
	else if(m_uLogType==LOG_TO_SCREEN)
	{
		if(InitConsole()==FALSE)
		return FALSE;
	}
	else
	{
		Error("��־���Ͳ���ʶ��\n");
		return FALSE;
	}

	InitializeCriticalSection(&m_csCriticalSection);
	m_bInited = TRUE;

	//Log(0,"��־ϵͳ���� %s\n",__TIME__);
	//Log(0,"��־ϵͳ���� %s\n",__DATE__);
	//Log(0,"��־ϵͳ���� %s\n",__TIMESTAMP__);

	return m_bInited;
}
	

/*****************************************************************************/
// �ر���־ϵͳ
// ��������
/*****************************************************************************/
BOOL CLog::ReleaseLogSystem(void)
{
	if(!m_bInited)
	{
		Error("��־ϵͳû�г�ʼ����\n");
		return TRUE;
	}

	if(m_hOutputConsole)
	{
		if(FreeConsole())
		{
			m_hOutputConsole = NULL;
		}
		else
		{
			Error("�ڲ����󣬹رտ���̨ʧ�ܣ�������� %d\n",GetLastError());
			//return FALSE;
		}
	}
	
	if(m_hOutputFile)//�����ļ����
	{
		CloseHandle(m_hOutputFile);
		m_hOutputFile = NULL;
	}

	m_bInited = 0;
	DeleteCriticalSection(&m_csCriticalSection);

	return TRUE;
}
	
/*****************************************************************************/
//��־���������������
// uLevel : ��־�ȼ���һ��3�֣���LOG_TO_SCREENģʽ�£���ͬ��־��ӡ��ɫ��ͬ
//          ��LOG_TO_FILEģʽ�£�����ÿ����־ǰ�Ķ�һ���ȼ���ǡ�
//           LOG_LEVEL_NOMAL   
//           LOG_LEVEL_WARNING
//           LOG_LEVEL_ERROR
// pszFormat:�������
/*****************************************************************************/
void CLog::Log(UINT uLevel,LPCSTR pszFormat, ... )
{
	if(!m_bInited) return;

	if(m_bLogDate)
	{
		//д���ݵ��ļ�(дʱ��)
		SYSTEMTIME	sTime;
		const int lenTime = 60;
		char tBuffer[lenTime]={0};
	
		GetLocalTime (&sTime);
		StringCbPrintfA(tBuffer,lenTime,"[%d]%d/%d/%d-%d:%d:%d \0",
			uLevel,sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
		Printf(uLevel,tBuffer);
	}

	va_list arglist;
	va_start (arglist, pszFormat);
	Lock();
	Printf(uLevel,vaFormat(pszFormat,arglist));
	UnLock();
	va_end (arglist);

};
