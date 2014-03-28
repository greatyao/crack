/*****************************************************************************
*
* CLog 日志类，用于打印输出日志信息，或者将日志信息保存到文件
* 完成时间 2013-12-26
* 版本     初始化版本
* 更新记录 无
*
*****************************************************************************/
#ifndef WIN32_LEAN_AND_MEAN
#define	WIN32_LEAN_AND_MEAN

#include <windows.h>


/*****************************************************************************
*
* 日志系统类
* 支持保存日志到文件，或者将日志保存到文件
* 支持不定参数提交单条日志
*
*****************************************************************************/

//日志等级
enum LOG_LEVEL
{
	LOG_LEVEL_NOMAL = 1,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
};

//日志类型
enum LOG_TYPE
{
	LOG_TO_FILE=1,
	LOG_TO_SCREEN
};

class CLog
{
private:
	static HANDLE m_hOutputConsole;	//标准输出控制台句柄
	static HANDLE m_hOutputFile;	//标准输出文件句柄
	static BOOL   m_bInited;		//初始化标记
	static UINT   m_uLogType;		//日志类型
	static BOOL   m_bLogDate;		//记录日期时间

	static CRITICAL_SECTION m_csCriticalSection;//临界区
	/*****************************************************************************/
	// 同步互斥锁机制
	/*****************************************************************************/
	static void Lock(void);
	static void UnLock(void);
	static BOOL TryLock(void);
	
	/*****************************************************************************/
	// 初始化屏幕输出控制台
	/*****************************************************************************/
	static BOOL InitConsole(void);

	/*****************************************************************************/
	// 初始化文件日志
	/*****************************************************************************/
	static BOOL InitFileSystem(const char *cFile);
	
	//打印输出函数(内部调用)
	static int Printf(UINT uLevel,const char * buffer);
	//写日志文件（内部调用）
	static int Write(UINT uLevel,const char * buffer);

	//内部日志输出（debug调试信息输出）
	static int Error(LPCSTR pszFormat, ... );
	
	static char * vaFormat(LPCSTR pszFormat,va_list arglist);

public:
	/*****************************************************************************/
	// 初始化日志系统
	// uType : 指定日志类型 
	//           LOG_TO_FILE   表示将日志写到文件
	//           LOG_TO_SCREEN 表示将日志打印到屏幕
	// bDate : 指定日志是否打印日志时间(含日期)
	// cFile : 指定输出日志的文件名，仅当 uType 为 LOG_TO_FILE有效
	/*****************************************************************************/
	static BOOL InitLogSystem(UINT uType = LOG_TO_SCREEN,BOOL bDate = FALSE,const char *cFile = NULL);
	

	/*****************************************************************************/
	// 关闭日志系统
	// 做清理工作，关闭的时候调用
	/*****************************************************************************/
	static BOOL ReleaseLogSystem(void);
	
	
	/*****************************************************************************/
	//日志（错误输出函数）
	// uLevel : 日志等级，一共3种，在LOG_TO_SCREEN模式下，不同日志打印颜色不同
	//          在LOG_TO_FILE模式下，仅在每条日志前的多一个等级标记。
	//           LOG_LEVEL_NOMAL   
	//           LOG_LEVEL_WARNING
	//           LOG_LEVEL_ERROR
	// pszFormat:输出内容格式或内容
	//     ...  :内容
	/*****************************************************************************/
	static void Log(UINT uLevel,LPCSTR pszFormat, ... );
};

#endif


