#pragma once

#include "sqlite3.h"
#include <stdio.h>
#include "CLog.h"
#include "CppSQLite3.h"
#include "CrackBroker.h"
#include "CompClient.h"


class CPersistencManager
{
public:
	CPersistencManager();
	~CPersistencManager(void);

	bool OpenDB(const char* name);

	int PersistTask(const CCrackTask* task, bool update = false);
	int PersistHash(const char* guid, const CRACK_HASH_LIST& hash);
	int PersistBlockMap(const CB_MAP& block_map);
	int PersistReadyTaskQueue(const CT_DEQUE& ready_list);
	int PersistClientInfo(const CI_VECTOR& client_list);
	int PersistNoticeMap(const CCB_MAP& notice_map);


	int LoadTaskMap(CT_MAP &task_map);
	int LoadHash(CT_MAP &task_map);
	int LoadBlockMap(CB_MAP &block_map,CT_MAP &task_map);
	int LoadReadyTaskQueue(CT_DEQUE &ready_list,CT_MAP task_map);
	int LoadClientInfo(CI_VECTOR &client_list);
	int LoadNoticeMap(CCB_MAP &notice_map);


private:

	CppSQLite3DB m_SQLite3DB;

	string m_TaskTable;
	string m_BlockTable;
	string m_HashTable;
	string m_NoticeTable ;
	string m_ReadyTaskTable ;
	string m_ClientTable ;

	int CreateTable();

};

extern CPersistencManager g_Persistence;