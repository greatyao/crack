#pragma once

#include <vector>

/*******************************************************************
//workitem状态标记
*******************************************************************/
enum{
	WI_STATUS_WAITING = 1,	//分配好任务以后，处于等待计算节点处理
	WI_STATUS_RUNNING,		//正在被计算节点处理中
	WI_STATUS_FINISHED,		//完成结束,包含有解密结果
	WI_STATUS_NOT_NEED,		//任务不需要处理

	WI_STATUS_STOPPED,			//任务被停止后，相关的工作项应该被停止
	WI_STATUS_PAUSED,		//任务被暂停后，相关的工作项应该被暂停
	WI_STATUS_DELETED,		//将工作项设置为删除状态

	WI_STATUS_NO_PWD,				//解密结束但是未找到密码
	WI_STATUS_FAILURE,		//工作项执行失败
	WI_STATUS_MAX
};



class CWorkItem
{

public:

	std::string m_string_wi_guid;	//workitem的guid

	std::string m_string_wi_john;	//目标破解的信息
	std::string m_string_wi_begin;	//暴力破解的开始字符
	std::string m_string_wi_end;	//暴力破解的结束字符
	std::string m_string_wi_chars_set;//暴力破解的字符集

	unsigned char m_wi_status;	//workitem当前状态
	std::string m_comp_guid; //分配的计算节点guid
	
	LPVOID	* m_ptask;	//指向本workitem 属于的任务


public:
	CWorkItem(void);
	~CWorkItem(void);

	/***************************************************************
	初始化workitem信息，并返回guid字符串
	***************************************************************/
	//string init(string sjohn,string sbegin,string send,string scset);
	/***************************************************************
	更新workitem状态
	***************************************************************/
	/*
	void update_status_to_running(void);
	void update_status_to_finished(void);
	void update_status_to_not_need(void);
	void update_status_to_waiting(void);
	*/
};
