#pragma once

#define STATUS_NOTICE_FINISH 1  //针对一个block 计算完成，其他不用计算的情况
#define STATUS_NOTICE_RESET 2   //针对计算节点断线，其正在运行的block 需要重置状态为Ready 
#define STATUS_NOTICE_STOP	4   //针对停止任务，通知计算节点该block终止运行

#define STATUS_NOTICE_RUN 8 //正在运行中的block状态

class CBlockNotice
{
public:
	CBlockNotice(void);
	~CBlockNotice(void);

public:

	char m_guid[40];
	char m_status;

};
