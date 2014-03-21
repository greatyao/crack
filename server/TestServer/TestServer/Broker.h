#pragma once

#include <vector>

#include "Task.h"

class CBroker
{
private:
//	RTL_CRITICAL_SECTION m_critical_section_broker;
//	void lock(void);
//	void unlock(void);

public:
	
	CBroker(void);
	~CBroker(void);
	std::vector <CTask *> m_coretask_list;

	//上传一个任务,返回任务的guid，或者空
//	string update_task(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon);
//	cworkitem *broker_get_work_item(void);
	
	//添加获取 guid 的任务的状态
//	unsigned  broker_get_coretask(string &string_guid);

	//暂停guid 的任务，并返回相关的状态和原因
//	unsigned  broker_pause_coretask(string &string_guid);

	//停止guid 的任务，并返回相关的状态和原因
//	unsigned  broker_stop_coretask(string &string_guid);

	//删除guid 的任务，并返回相关的状态和原因
//	unsigned  broker_delete_coretask(string &string_guid);

};
