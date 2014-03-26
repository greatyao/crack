/*****************************************************************************
*
* CLog ��־�࣬���ڴ�ӡ�����־��Ϣ�����߽���־��Ϣ���浽�ļ�
* ���ʱ�� 2013-12-26
* �汾     ��ʼ���汾
* ���¼�¼ ��
*
*****************************************************************************/
#ifndef WIN32_LEAN_AND_MEAN
#define	WIN32_LEAN_AND_MEAN

#include <windows.h>


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
	LOG_LEVEL_ERROR
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
	static HANDLE m_hOutputConsole;	//��׼�������̨���
	static HANDLE m_hOutputFile;	//��׼����ļ����
	static BOOL   m_bInited;		//��ʼ�����
	static UINT   m_uLogType;		//��־����
	static BOOL   m_bLogDate;		//��¼����ʱ��

	static CRITICAL_SECTION m_csCriticalSection;//�ٽ���
	/*****************************************************************************/
	// ͬ������������
	/*****************************************************************************/
	static void Lock(void);
	static void UnLock(void);
	static BOOL TryLock(void);
	
	/*****************************************************************************/
	// ��ʼ����Ļ�������̨
	/*****************************************************************************/
	static BOOL InitConsole(void);

	/*****************************************************************************/
	// ��ʼ���ļ���־
	/*****************************************************************************/
	static BOOL InitFileSystem(const char *cFile);
	
	//��ӡ�������(�ڲ�����)
	static int Printf(UINT uLevel,const char * buffer);
	//д��־�ļ����ڲ����ã�
	static int Write(UINT uLevel,const char * buffer);

	//�ڲ���־�����debug������Ϣ�����
	static int Error(LPCSTR pszFormat, ... );
	
	static char * vaFormat(LPCSTR pszFormat,va_list arglist);

public:
	/*****************************************************************************/
	// ��ʼ����־ϵͳ
	// uType : ָ����־���� 
	//           LOG_TO_FILE   ��ʾ����־д���ļ�
	//           LOG_TO_SCREEN ��ʾ����־��ӡ����Ļ
	// bDate : ָ����־�Ƿ��ӡ��־ʱ��(������)
	// cFile : ָ�������־���ļ��������� uType Ϊ LOG_TO_FILE��Ч
	/*****************************************************************************/
	static BOOL InitLogSystem(UINT uType = LOG_TO_SCREEN,BOOL bDate = FALSE,const char *cFile = NULL);
	

	/*****************************************************************************/
	// �ر���־ϵͳ
	// �����������رյ�ʱ�����
	/*****************************************************************************/
	static BOOL ReleaseLogSystem(void);
	
	
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
	static void Log(UINT uLevel,LPCSTR pszFormat, ... );
};

#endif


