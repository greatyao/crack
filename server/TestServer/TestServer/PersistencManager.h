#pragma once

#include "sqlite3.h"
#include "leveldb.h"
#include <stdio.h>
#include "CppSQLite3.h"
#include "CompClient.h"


class CPersistencManager
{
public:
	enum Action{Insert, Update, Delete};


	CPersistencManager();
	~CPersistencManager(void);

	bool OpenDB(const char* name, bool use_leveldb = false);

	int PersistTask(const CCrackTask* task, Action action);

	int PersistHash(const char* guid, const CRACK_HASH_LIST& hash, Action action);
	int PersistBlockMap(const CB_MAP& block_map, Action action);
	int PersistReadyTaskQueue(const CT_DEQUE& ready_list);
	int PersistClientInfo(const CI_VECTOR& client_list);
	int PersistNoticeMap(const CCB_MAP& notice_map);


	int LoadTaskMap(CT_MAP &task_map);
	int LoadHash(CT_MAP &task_map);
	int LoadBlockMap(CB_MAP &block_map,CT_MAP &task_map);
	int LoadReadyTaskQueue(CT_DEQUE &ready_list, const CT_MAP& task_map);
	int LoadClientInfo(CI_VECTOR &client_list);
	int LoadNoticeMap(CCB_MAP &notice_map);


private:

	CppSQLite3DB m_SQLite3DB;
	leveldb::DB* m_LevelDB;
	bool m_useLevelDB;

	string m_TaskTable;
	string m_BlockTable;
	string m_HashTable;
	string m_NoticeTable ;
	string m_ReadyTaskTable ;
	string m_ClientTable ;

	int CreateTable();

};

extern CPersistencManager g_Persistence;