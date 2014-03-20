#pragma once


#include <afxwin.h>         // MFC 核心组件和标准组件
#include <string>
#include <vector>
#include <iostream>
#include "guidgenerator.h"
#include "WorkItem.h"


/*******************************************************************
//coretask状态（暂时未使用）
*******************************************************************/
typedef enum TASK_STATUS{

	//添加了任务状态
	CT_STATUS_READY = 1,   //任务的就绪状态
	CT_STATUS_FINISHED,		//任务完成状态	
	CT_STATUS_FAILURE,		//任务解密失败状态
	CT_STATUS_RUNNING,		//任务正在解密状态
	CT_STATUS_PAUSED,		//任务暂停解密状态


	CT_STATUS_DELETED,		//将任务设置为删除状态
	CT_STATUS_MAX

};


typedef enum TASK_PRIORITY{

	//任务的优先级
	TASK_PRIORITY_NORMAL = 1,
	TASK_PRIORITY_IMPORTANT,
	

};


class CTask
{

public:
	std::string m_string_ct_guid;//coretask guid

	unsigned m_split_number;	//切割任务数
	unsigned m_finished_number;	//完成任务数

	bool m_crack_success;		//破解是否成功	
	std::string m_string_result;//成功破解到的密码
	float m_progress;			//完成度 0.0~100.0;

	unsigned char m_task_status;			//任务的运行状态,对应Ready| Running | finished |
	unsigned char priority;		//任务优先级

	std::string m_algtype;		//算法类型
	std::string m_hashinfo;		//Hash信息
	std::string m_ctlguid;		//控制节点guid

	
	std::vector <CWorkItem *> m_workitem_list;//workitem子项
	
	/***************************************************************
	构造函数，初始化一些基本信息
	***************************************************************/
//	ccoretask();
	/***************************************************************
	直接返回guid字符串
	***************************************************************/	
//	string init(void);	
	/***************************************************************
	析构函数，清理资源信息
	***************************************************************/
//	~ccoretask();

	//coretask 有关操作都在这里
	/***************************************************************
	获取进度信息
	***************************************************************/
//	float get_progress(void);
	/***************************************************************
	插入一个workitem
	***************************************************************/
//	void insert_work_item(cworkitem *workitem);
	/***************************************************************
	获取一个等待运算的workitem
	***************************************************************/
//	cworkitem *get_work_item(void);
	/***************************************************************
	更新一个workitem
	***************************************************************/
//	void update_work_item(string &sguid);

public:
	CTask(void);
	~CTask(void);
};
