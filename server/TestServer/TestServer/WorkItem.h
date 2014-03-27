#pragma once

#include <vector>
#include "algorithm_types.h"

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
	
	//New workitem define
/*	unsigned char m_algo;		//解密算法
	unsigned char m_charset;	//解密字符集
	unsigned char m_type;		//解密类型
	unsigned char m_special;	//是否是文件解密（pdf+office+rar+zip）
	char m_guid[40];			//服务端的workitem的GUID
	char m_john[sizeof(struct crack_hash)];		//原始Hash格式：hash值+盐
	unsigned short m_start;	//开始长度
	unsigned short m_end;		//结束长度
	//以下两个是索引
	unsigned short m_start2;	//55555-99999:start2=5,end2=9	000-55555:start2=0,end2=5
	unsigned short m_end2;
	char m_custom[0]; //用户自定义的字符集
*/
	//Old workitem define
	
	std::string m_string_wi_guid;	//workitem的guid

	std::string m_string_wi_john;	//目标破解的信息
	std::string m_string_wi_begin;	//暴力破解的开始字符
	std::string m_string_wi_end;	//暴力破解的结束字符
	std::string m_string_wi_chars_set;//暴力破解的字符集

	unsigned char m_wi_status;	//workitem当前状态
	std::string m_comp_guid; //分配的计算节点guid

	
	void * m_ptask;	//指向本workitem 属于的任务


public:
	CWorkItem(void);
	~CWorkItem(void);

	/***************************************************************
	初始化workitem信息，并返回guid字符串
	***************************************************************/
	//std::string init(std::string sjohn,std::string sbegin,std::string send,std::string scset);
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
