#pragma once

#include "sqlite3.h"
#include <stdio.h>
#include "CLog.h"
#include "CppSQLite3.h"
#include "CrackBroker.h"
#include "CompClient.h"


typedef (int (*clientCallBack)(void *, int , char **, char **)) DB_READ_CALLBACK;

class CPersistencManager
{
public:
	CPersistencManager(char *pDB);
	~CPersistencManager(void);

	int CreateTable();


	int PersistTaskMap(CT_MAP task_map);
	int PersistHash(CT_MAP task_map);
	int PersistBlockMap(CB_MAP block_map);

	int PersistReadyTaskQueue(CT_DEQUE ready_list);
	int PersistClientInfo(CI_VECTOR client_list);
	int PersistNoticeMap(CCB_MAP notice_map);


	int LoadTaskMap(CT_MAP &task_map);
	int LoadHash(CT_MAP &task_map);
	int LoadBlockMap(CB_MAP &block_map);
	int LoadReadyTaskQueue(CT_DEQUE &ready_list);
	int LoadClientInfo(CI_VECTOR &client_list);
	int LoadNoticeMap(CCB_MAP &notice_map);


private:


	int execNoQuerySQL(char *cmdsql);
	int execQuerySQL(char *querysql,DB_READ_CALLBACK cbfunc);

private:

	CppSQLite3DB m_SQLite3DB;

	string m_TaskTable = "create table Task (taskid char(40),algo char(1),charset char(1),type char(1),filetag char(1),"
		"single char(1),startlength int,endlength int,owner char(64),status char(1),splitnum int,finishnum int,success char(1),progress real,speed real,starttime char(20),runtime int,remaintime int,count int)";
	string m_BlockTable = "create table Hash (taskid char(40),index int,john char(260),result char(32),status char(1),progress real)";
	string m_HashTable = "create table Block (blockid char(40),taskid char(40),index int,status char(1),progress real,spead real,remaintime int,compip char(20))";
	string m_NoticeTable = "create table Notice (hostip char(20),blockid char(40),status char(1))";
	string m_ReadyTaskTable = "create table ReadyTask (taskid char(40))";
	string m_ClientTable = "create table Client (ip char(20),type char(1),hostname char(64),osinfo char(64),livetime char(20),logintime char(20),gpu int,cpu int)";


};
