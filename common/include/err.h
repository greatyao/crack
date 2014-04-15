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

#endif
