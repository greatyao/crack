#include "CLog.h"
#include <stdarg.h>
#include <string.h>
#if defined(WIN32) || defined(WIN64)
#pragma warning (disable:4995)
#include <Strsafe.h>
#else
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
#endif

bool   CLog::m_bInited = false;
bool   CLog::m_bLogDate = false;
unsigned int   CLog::m_uLogType = LOG_TO_SCREEN;
#if defined(WIN32) || defined(WIN64) 
HANDLE CLog::m_hOutputConsole = NULL;
#endif
FILE* CLog::m_hOutputFile = NULL;
pthread_mutex_t CLog::m_mutex;

/*****************************************************************************/
// ͬ�����������ƣ��ڲ����ã�
/*****************************************************************************/
//��
void CLog::Lock(void)
{
	pthread_mutex_lock(&m_mutex);
}
//����
void CLog::UnLock(void)
{
	pthread_mutex_unlock(&m_mutex);
}
//������
bool CLog::TryLock(void)
{
	return pthread_mutex_trylock(&m_mutex);
}

/*****************************************************************************/
// ��ʼ����Ļ�������̨(�ڲ�����)
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
// ��ʼ���ļ���־
/*****************************************************************************/
bool CLog::InitFileSystem(const char *cFile)
{
	m_hOutputFile = fopen(cFile, "w+");
	if( m_hOutputFile==NULL )
	{
		return false;
	}
	return true;
}
	
//��ӡ�������(�ڲ�����)
int CLog::Printf(unsigned int uLevel,const char * buffer)
{
	if(m_uLogType==LOG_TO_FILE)
		return Write(uLevel,buffer);
#if defined(WIN32) || defined(WIN64)
	DWORD cbWritten;
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
	return cbWritten;
#else
	if(uLevel == LOG_LEVEL_NOMAL)
		hlog("%s", buffer);
	else if(uLevel == LOG_LEVEL_WARNING)
		wlog("%s", buffer);
	else
		elog("%s", buffer);

#endif
}
//д��־�ļ����ڲ����ã�
int CLog::Write(unsigned int uLevel,const char * buffer)
{
	int n = fprintf(m_hOutputFile, "%s", buffer);
	fflush(m_hOutputFile);
	return n;
}

/*****************************************************************************/
// ��ʼ����־ϵͳ
// uType : ָ����־���� 
//           LOG_TO_FILE   ��ʾ����־д���ļ�
//           LOG_TO_SCREEN ��ʾ����־��ӡ����Ļ
// bDate : ָ����־�Ƿ��ӡ��־ʱ��(������)
// cFile : ָ�������־���ļ��������� uType Ϊ LOG_TO_FILE��Ч
/*****************************************************************************/
bool CLog::InitLogSystem(unsigned int uType ,bool bDate ,const char *cFile )
{
	if(m_bInited)
	{	//�Ѿ���ʼ����,����趨Ϊ���Ը��ĳ�ʼ����Ϣ����Ҫ�����Ϣ�����³�ʼ��
		return true;
	}

	m_uLogType = uType;
	m_bLogDate = bDate;

	if(m_uLogType==LOG_TO_FILE)
	{
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

	pthread_mutex_init(&m_mutex, NULL);
	m_bInited = true;

	return m_bInited;
}
	

/*****************************************************************************/
// �ر���־ϵͳ
// ��������
/*****************************************************************************/
bool CLog::ReleaseLogSystem(void)
{
	if(!m_bInited)
	{
		return true;
	}
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
	
	if(m_hOutputFile)//�����ļ����
	{
		fclose(m_hOutputFile);
		m_hOutputFile = NULL;
	}
#endif
	m_bInited = 0;
	pthread_mutex_destroy(&m_mutex);

	return true;
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
void CLog::Log(unsigned int uLevel, const char* pszFormat, ... )
{
	if(!m_bInited) return;
	
	const int len_buffer = 1024 * 4;
	char	buffer[len_buffer] = {0};	
	if(m_bLogDate)
	{
		//д���ݵ��ļ�(дʱ��)
		time_t t;									
		struct tm *tm1;								
		time(&t);									
		tm1 = localtime(&t);						
		strftime(buffer, 256, "%Y-%m-%d %H:%M:%S ", tm1);
	}

	va_list arglist;
	va_start (arglist, pszFormat);
	vsprintf(buffer+strlen(buffer), pszFormat, arglist);

	Lock();
	Printf(uLevel, buffer);
	UnLock();
	va_end (arglist);

};
