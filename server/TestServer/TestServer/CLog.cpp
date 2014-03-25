
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
// 同步互斥锁机制（内部调用）
/*****************************************************************************/
//锁
void CLog::Lock(void)
{
	EnterCriticalSection(&m_csCriticalSection);
}
//解锁
void CLog::UnLock(void)
{
	LeaveCriticalSection(&m_csCriticalSection);
}
//尝试锁
BOOL CLog::TryLock(void)
{
	return TryEnterCriticalSection(&m_csCriticalSection);
}

/*****************************************************************************/
// 初始化屏幕输出控制台(内部调用)
/*****************************************************************************/
BOOL CLog::InitConsole(void)
{
	if(m_hOutputConsole) 
		return TRUE;

	AllocConsole();
	/*
	if(AllocConsole()==FALSE)
	{
		Error("致命错误，分配控制台失败，错误代码 %d\n",GetLastError());
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
// 初始化文件日志
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
		Error("创建日志文件(%s)失败,错误代码 %d\n",cFile,GetLastError());
		return FALSE;
	}
	return TRUE;
}
	
//打印输出函数(内部调用)
int CLog::Printf(UINT uLevel,const char * buffer)
{
	DWORD cbWritten;

	if(m_uLogType==LOG_TO_FILE)
	{
		cbWritten = Write(uLevel,buffer);
	}
	else{
		//保存控制台属性	
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		WORD saved_attributes;

		//获取原有属性
		GetConsoleScreenBufferInfo(m_hOutputConsole, &consoleInfo);
		saved_attributes = consoleInfo.wAttributes;

		//设置颜色
		WORD wColor = 7;
		if( LOG_LEVEL_WARNING== uLevel)
			wColor = 11;
		if( LOG_LEVEL_ERROR== uLevel)
			wColor = 12;
		SetConsoleTextAttribute(m_hOutputConsole, wColor);
		//输出		
		WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), buffer, lstrlenA(buffer),&cbWritten, 0 );
		//恢复
		SetConsoleTextAttribute(m_hOutputConsole, saved_attributes);
	}
	return cbWritten;
}
//写日志文件（内部调用）
int CLog::Write(UINT uLevel,const char * buffer)
{
	DWORD dwWritten;
	SetFilePointer(m_hOutputFile, 0, 0, FILE_END);
	WriteFile(m_hOutputFile, (LPCVOID)buffer, lstrlenA(buffer), &dwWritten, NULL);
	return dwWritten;
}

//内部日志输出（debug调试信息输出）
int CLog::Error(LPCSTR pszFormat, ... )
{
	const int len_buffer = 1024 * 1;//1k 缓存
	char	buffer[len_buffer];	

	va_list arglist;
	va_start (arglist, pszFormat);
	ZeroMemory(buffer,len_buffer);
	StringCchVPrintfA(buffer,len_buffer,pszFormat,arglist);
	va_end (arglist);

	OutputDebugStringA( buffer );
	return 0;
}
	
//va_list 格式化
char * CLog::vaFormat(LPCSTR pszFormat,va_list arglist)
{
	const int len_buffer = 1024 * 8;//8k 缓存
	static char	buffer[len_buffer];	

	ZeroMemory(buffer,len_buffer);
	StringCchVPrintfA(buffer,len_buffer,pszFormat,arglist);
	return buffer;
}
/*****************************************************************************/
// 初始化日志系统
// uType : 指定日志类型 
//           LOG_TO_FILE   表示将日志写到文件
//           LOG_TO_SCREEN 表示将日志打印到屏幕
// bDate : 指定日志是否打印日志时间(含日期)
// cFile : 指定输出日志的文件名，仅当 uType 为 LOG_TO_FILE有效
/*****************************************************************************/
BOOL CLog::InitLogSystem(UINT uType ,BOOL bDate ,const char *cFile )
{
	if(m_bInited)
	{	//已经初始化过,如果设定为可以更改初始化信息，需要清除信息并重新初始化
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
		Error("日志类型不可识别\n");
		return FALSE;
	}

	InitializeCriticalSection(&m_csCriticalSection);
	m_bInited = TRUE;

	//Log(0,"日志系统启动 %s\n",__TIME__);
	//Log(0,"日志系统启动 %s\n",__DATE__);
	//Log(0,"日志系统启动 %s\n",__TIMESTAMP__);

	return m_bInited;
}
	

/*****************************************************************************/
// 关闭日志系统
// 做清理工作
/*****************************************************************************/
BOOL CLog::ReleaseLogSystem(void)
{
	if(!m_bInited)
	{
		Error("日志系统没有初始化过\n");
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
			Error("内部错误，关闭控制台失败，错误代码 %d\n",GetLastError());
			//return FALSE;
		}
	}
	
	if(m_hOutputFile)//处理文件句柄
	{
		CloseHandle(m_hOutputFile);
		m_hOutputFile = NULL;
	}

	m_bInited = 0;
	DeleteCriticalSection(&m_csCriticalSection);

	return TRUE;
}
	
/*****************************************************************************/
//日志（错误输出函数）
// uLevel : 日志等级，一共3种，在LOG_TO_SCREEN模式下，不同日志打印颜色不同
//          在LOG_TO_FILE模式下，仅在每条日志前的多一个等级标记。
//           LOG_LEVEL_NOMAL   
//           LOG_LEVEL_WARNING
//           LOG_LEVEL_ERROR
// pszFormat:输出内容
/*****************************************************************************/
void CLog::Log(UINT uLevel,LPCSTR pszFormat, ... )
{
	if(!m_bInited) return;

	if(m_bLogDate)
	{
		//写数据到文件(写时间)
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
