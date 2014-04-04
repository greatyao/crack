/*****************************************************************************
*
* CLog ��־�࣬���ڴ�ӡ�����־��Ϣ�����߽���־��Ϣ���浽�ļ�
* ���ʱ�� 2013-12-26
* �汾     ��ʼ���汾
* ���¼�¼ ��
*
*****************************************************************************/
#ifndef _CLOG_H_
#define	_CLOG_H_

#if defined(WIN32) || defined(WIN64) 
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>

/*****************************************************************************
*
* ��־ϵͳ��
* ֧�ֱ�����־���ļ������߽���־���浽�ļ�
* ֧�ֲ��������ύ������־
*
*****************************************************************************/

//��־�ȼ�
enum LOG_LEVEL
{
	LOG_LEVEL_NOMAL = 1,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_NOTICE,
	LOG_LEVEL_SUCCEED
};

//��־����
enum LOG_TYPE
{
	LOG_TO_FILE=1,
	LOG_TO_SCREEN
};

class CLog
{
private:
#if defined(WIN32) || defined(WIN64) 
	static HANDLE m_hOutputConsole;	//��׼�������̨���
	static CRITICAL_SECTION m_cs;
#else
	static pthread_mutex_t m_mutex;//�ٽ���
#endif
	static FILE*  m_hOutputFile;	//��׼����ļ����
	static bool   m_bInited;		//��ʼ�����
	static unsigned int   m_uLogType;		//��־����
	static bool   m_bLogDate;		//��¼����ʱ��	
	static int lastDay, currDay;
	static char suffix[256];
	/*****************************************************************************/
	// ͬ������������
	/*****************************************************************************/
	static void Lock(void);
	static void UnLock(void);
	static bool TryLock(void);
	
	/*****************************************************************************/
	// ��ʼ����Ļ�������̨
	/*****************************************************************************/
	static bool InitConsole(void);

	/*****************************************************************************/
	// ��ʼ���ļ���־
	/*****************************************************************************/
	static bool InitFileSystem(const char *cFile);
	
	//��ӡ�������(�ڲ�����)
	static int Printf(unsigned int uLevel,const char * buffer);
	//д��־�ļ����ڲ����ã�
	static int Write(unsigned int uLevel,const char * buffer);
public:
	/*****************************************************************************/
	// ��ʼ����־ϵͳ
	// uType : ָ����־���� 
	//           LOG_TO_FILE   ��ʾ����־д���ļ�
	//           LOG_TO_SCREEN ��ʾ����־��ӡ����Ļ
	// bDate : ָ����־�Ƿ��ӡ��־ʱ��(������)
	// cFile : ָ�������־���ļ��������� uType Ϊ LOG_TO_FILE��Ч
	/*****************************************************************************/
	static bool InitLogSystem(unsigned int uType = LOG_TO_SCREEN,bool bDate = false,const char *cFile = NULL);
	

	/*****************************************************************************/
	// �ر���־ϵͳ
	// �����������رյ�ʱ�����
	/*****************************************************************************/
	static bool ReleaseLogSystem(void);
	
	
	/*****************************************************************************/
	//��־���������������
	// uLevel : ��־�ȼ���һ��3�֣���LOG_TO_SCREENģʽ�£���ͬ��־��ӡ��ɫ��ͬ
	//          ��LOG_TO_FILEģʽ�£�����ÿ����־ǰ�Ķ�һ���ȼ���ǡ�
	//           LOG_LEVEL_NOMAL   
	//           LOG_LEVEL_WARNING
	//           LOG_LEVEL_ERROR
	// pszFormat:������ݸ�ʽ������
	//     ...  :����
	/*****************************************************************************/
	static void Log(unsigned int uLevel, const char* pszFormat, ... );
};

#endif