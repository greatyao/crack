#include "CLog.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

#if defined(__linux__) || defined(__CYGWIN__)
#define EWHERESTR  "\033[1;31m[crackctrl]\033[0m "
#define EWHEREARG  __FILE__, __LINE__
#define elogerr(...)     fprintf(stderr, __VA_ARGS__)
#define elog(_fmt, ...)  elogerr(EWHERESTR _fmt, __VA_ARGS__)

#define WWHERESTR  "\033[1;33m[crackctrl]\033[0m "
#define wlogwarn(...)     fprintf(stderr, __VA_ARGS__)
#define wlog(_fmt, ...)  wlogwarn(WWHERESTR _fmt, __VA_ARGS__)

#define HWHERESTR  "\033[1m[crackctrl]\033[0m "
#define hlogstd(...)     fprintf(stderr, __VA_ARGS__)
#define hlog(_fmt, ...)  hlogstd(HWHERESTR _fmt,  __VA_ARGS__)

#define NWHERESTR  "\033[1;35m[crackctrl]\033[0m "
#define nlogstd(...)     fprintf(stderr, __VA_ARGS__)
#define nlog(_fmt, ...)  nlogstd(NWHERESTR _fmt,  __VA_ARGS__)

#define NW2HERESTR  "\033[1;32m[crackctrl]\033[0m "
#define n2logstd(...)     fprintf(stderr, __VA_ARGS__)
#define n2log(_fmt, ...)  n2logstd(NW2HERESTR _fmt,  __VA_ARGS__)
#endif

#if defined(WIN32) || defined(WIN64) 
HANDLE CLog::m_hOutputConsole = NULL;
CRITICAL_SECTION CLog::m_cs;
#else
pthread_mutex_t CLog::m_mutex;
#endif
bool   CLog::m_bInited = false;
bool   CLog::m_bLogDate = false;
unsigned int   CLog::m_uLogType = LOG_TO_SCREEN;
FILE* CLog::m_hOutputFile = NULL;
int CLog::currDay;
int CLog::lastDay;
char CLog::suffix[256] = {0};

static int GetDay(char* buffer, int size, bool hms)
{
	time_t nowtime;
	struct tm* ptm;     
	time(&nowtime);     
	ptm = localtime(&nowtime);
	if(buffer)
	{
		if(hms)	strftime(buffer, size, "%Y-%m-%d %H:%M:%S ", ptm);	
		else	strftime(buffer, size, "%Y-%m-%d", ptm);	
	}
		
	return ptm->tm_mday;
}

/*****************************************************************************/
// 同步互斥锁机制（内部调用）
/*****************************************************************************/
//锁
void CLog::Lock(void)
{
#if defined(WIN32) || defined(WIN64) 
	EnterCriticalSection(&m_cs);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}
//解锁
void CLog::UnLock(void)
{
#if defined(WIN32) || defined(WIN64) 
	LeaveCriticalSection(&m_cs);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}
//尝试锁
bool CLog::TryLock(void)
{
#if defined(WIN32) || defined(WIN64) 
	return TryEnterCriticalSection(&m_cs);
#else
	return pthread_mutex_trylock(&m_mutex);
#endif
}

/*****************************************************************************/
// 初始化屏幕输出控制台(内部调用)
/*****************************************************************************/
bool CLog::InitConsole(void)
{
#if defined(WIN32) || defined(WIN64) 
	if(m_hOutputConsole) 
		return true;

	AllocConsole();

	m_hOutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(m_hOutputConsole) 
	{
		return true;
	}
	return false;
#else
	return true;
#endif
}


/*****************************************************************************/
// 初始化文件日志
/*****************************************************************************/
bool CLog::InitFileSystem(const char *cFile)
{
	char text[64];
	lastDay = GetDay(text, sizeof(text), false);
	
	FILE* f = m_hOutputFile;
	
	char logFile[512];
	sprintf(logFile, "%s-%s.log", suffix, text);
	m_hOutputFile = fopen(logFile, "a+");
	if( m_hOutputFile==NULL )
	{
		if(f) fprintf(f, "==Canot create logfile %s==\n", logFile);
		m_hOutputFile = f;
	}
	else
	{
		if(f)	fclose(f);
	}


	return m_hOutputFile;
}
	
//打印输出函数(内部调用)
int CLog::Printf(unsigned int uLevel,const char * buffer)
{
	if(m_uLogType==LOG_TO_FILE)
	{
		char text[64];
		currDay = GetDay(text, sizeof(text), true);
		if(currDay != lastDay)
		{
			fprintf(m_hOutputFile, "%sA new day is coming, turn off this log\n", text);
			InitFileSystem(NULL);
			fprintf(m_hOutputFile, "%sA new day and a new start\n", text);
		}
	
		return Write(uLevel,buffer);
	}
#if defined(WIN32) || defined(WIN64)
	DWORD cbWritten;
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
	return cbWritten;
#else
	if(uLevel == LOG_LEVEL_NOMAL)
		hlog("%s", buffer);
	else if(uLevel == LOG_LEVEL_WARNING)
		wlog("%s", buffer);
	else if(uLevel == LOG_LEVEL_ERROR)
		elog("%s", buffer);
	else if(uLevel == LOG_LEVEL_NOTICE)
		nlog("%s", buffer);
	else
		n2log("%s", buffer);

#endif
}
//写日志文件（内部调用）
int CLog::Write(unsigned int uLevel,const char * buffer)
{
	int n = fprintf(m_hOutputFile, "%s", buffer);
	fflush(m_hOutputFile);
	return n;
}

/*****************************************************************************/
// 初始化日志系统
// uType : 指定日志类型 
//           LOG_TO_FILE   表示将日志写到文件
//           LOG_TO_SCREEN 表示将日志打印到屏幕
// bDate : 指定日志是否打印日志时间(含日期)
// cFile : 指定输出日志的文件名，仅当 uType 为 LOG_TO_FILE有效
/*****************************************************************************/
bool CLog::InitLogSystem(unsigned int uType ,bool bDate ,const char *cFile )
{
	if(m_bInited)
	{	//已经初始化过,如果设定为可以更改初始化信息，需要清除信息并重新初始化
		return true;
	}

	m_uLogType = uType;
	m_bLogDate = bDate;

	if(m_uLogType==LOG_TO_FILE)
	{
#if defined(WIN32) || defined(WIN64) 
		char temp[MAX_PATH];
		GetModuleFileNameA(NULL, temp, sizeof(temp));
		int i;
		for(i = strlen(temp)-1; i>=0; i--)
		{	if(temp[i] == '\\' || temp[i] == '/')
			{
				temp[i+1] = 0;
				break;
			}
		}
		strncpy(suffix, temp, sizeof(suffix));

		for(i = strlen(cFile)-1; i>=0; i--)
		{
			if(cFile[i] == '\\' || cFile[i] == '/')
				break;
		}
		strncat(suffix, cFile+i+1, sizeof(suffix));
#else
		strncpy(suffix, cFile, sizeof(suffix));
#endif
		if(InitFileSystem(cFile)==false)
		return false;		
	}
	else if(m_uLogType==LOG_TO_SCREEN)
	{
		if(InitConsole()==false)
		return false;
	}
	else
	{
		return false;
	}

#if defined(WIN32) || defined(WIN64) 
	InitializeCriticalSection(&m_cs);
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif
	m_bInited = true;

	Log(LOG_LEVEL_NOMAL, "======== System starting now ======\n");
	
	return m_bInited;
}
	

/*****************************************************************************/
// 关闭日志系统
// 做清理工作
/*****************************************************************************/
bool CLog::ReleaseLogSystem(void)
{
	if(!m_bInited)
	{
		return true;
	}
	
	Log(LOG_LEVEL_NOMAL, "======== System stopped now ======\n");
	
#if defined(WIN32) || defined(WIN64)
	if(m_hOutputConsole)
	{
		if(FreeConsole())
		{
			m_hOutputConsole = NULL;
		}
		else
		{
			return false;
		}
	}
	
	if(m_hOutputFile)//处理文件句柄
	{
		fclose(m_hOutputFile);
		m_hOutputFile = NULL;
	}
	DeleteCriticalSection(&m_cs);
#else
	pthread_mutex_destroy(&m_mutex);	
#endif
	m_bInited = 0;

	return true;
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
void CLog::Log(unsigned int uLevel, const char* pszFormat, ... )
{
	if(!m_bInited) return;
	
	const int len_buffer = 1024 * 4;
	char	buffer[len_buffer] = {0};	
	if(m_bLogDate)
	{
		//写数据到文件(写时间)
		GetDay(buffer, sizeof(buffer), true);
	}

	va_list arglist;
	va_start (arglist, pszFormat);
	vsprintf(buffer+strlen(buffer), pszFormat, arglist);
	if(buffer[strlen(buffer)-1] != '\n')
		strcat(buffer, "\n");

	Lock();
	Printf(uLevel, buffer);
	UnLock();
	va_end (arglist);

};
