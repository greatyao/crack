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

	//�ϴ�һ������,���������guid�����߿�
//	string update_task(unsigned min_len,unsigned max_len,string &string_chars_set,unsigned split,const char *jhon);
//	cworkitem *broker_get_work_item(void);
	
	//��ӻ�ȡ guid �������״̬
//	unsigned  broker_get_coretask(string &string_guid);

	//��ͣguid �����񣬲�������ص�״̬��ԭ��
//	unsigned  broker_pause_coretask(string &string_guid);

	//ֹͣguid �����񣬲�������ص�״̬��ԭ��
//	unsigned  broker_stop_coretask(string &string_guid);

	//ɾ��guid �����񣬲�������ص�״̬��ԭ��
//	unsigned  broker_delete_coretask(string &string_guid);

};
