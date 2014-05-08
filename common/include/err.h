#ifndef _ERR_H_
#define _ERR_H_


#define ERR_SUCCEED				0
#define ERR_INVALID_PARAM		-1		//参数非法
#define ERR_NO_THISTASK			-2		//没有该任务
#define ERR_NO_SUPPORT_ALGO		-3		//不支持该解密类型	
#define ERR_NO_SUPPORT_CHARSET	-4		//不支持该字符集
#define ERR_FAILED_KILL			-5		//终止失败
#define ERR_LAUCH_TASK			-6		//启动失败
#define ERR_COMPRESS			-7		//压缩数据失败
#define ERR_UNCOMPRESS			-8		//解压缩数据失败
#define ERR_INVALIDDATA			-9		//从socket端接收到不正确的格式数据
#define ERR_CONNECTIONLOST		-10		//与socket断开连接
#define ERR_INTERNALCLIENT		-11		//Client内部错误
#define ERR_TIMEOUT				-12		//超时
#define ERR_FILENOEXIST			-13		//文件打开失败
#define ERR_CHILDEXIT			-14		//子进程已经结束
#define ERR_NOENTRY				-15		//配置文件中没有该字段的相关信息
#define ERR_DOWNLOADFILE		-16		//下载文件错误
#define ERR_NO_SUPPORT_TYPE		-17		//不支持该破解类型
#define ERR_NO_DICT				-18		//不支持指定的字典
#define ERR_FAILED_MKDIR		-19		//创建目录失败
#define ERR_AUTHENTICATION		-20		//验证失败
#define ERR_PRIVILEDGE			-21		//权限不够
#define ERR_WRONGHASHES			-22		//HASH格式错误
#define ERR_WRONGFILE			-23     //文件格式错误
#define ERR_SPLITTASK			-24		//切分任务错误
#define ERR_OUTOFMEMORY			-25		//内存不够
#define ERR_CONVERTREADY		-26		//不能将任务转换成就绪
#define ERR_CONVERTRUNNING		-27		//不能将任务转换成运行
#define ERR_CONVERTPAUSE		-28		//不能将任务转换成暂停
#define ERR_CONVERTSTOP			-29		//不能将任务转换成停止
#define ERR_CONVERTFAIL			-30		//不能将任务转换成失败
#define ERR_NOSUPPORTSTATUS		-31		//不支持该状态的任务切换
#define ERR_CONVERTDELETE		-32		//不能删除任务
#define ERR_CREATETASK			-33		//创建任务失败
#define ERR_NOREADYITEM			-34		//没有可用的workitem
#define ERR_NO_THISITEM			-35		//没有该workitem

#endif
