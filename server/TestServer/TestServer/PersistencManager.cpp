#include "PersistencManager.h"


CPersistencManager::CPersistencManager(char *pDB)
{
	int ret = 0;

	try{
		m_SQLite3DB.open(pDB);
	}catch(CppSQLite3Exception& ex){

		CLog::Log(LOG_LEVEL_WARNING,"Open Database :%s Error\n",pDB);
	}

	 m_TaskTable = "create table Task (taskid char(40),algo char(1),charset char(1),type char(1),filetag char(1),single char(1),startlength int,endlength int,owner char(64),status char(1),splitnum int,finishnum int,success char(1),progress real,speed real,starttime char(20),runtime int,remaintime int,count int)";
	 m_BlockTable = "create table Hash (taskid char(40),index int,john char(260),result char(32),status char(1),progress real)";
	 m_HashTable = "create table Block (blockid char(40),taskid char(40),index int,status char(1),progress real,spead real,remaintime int,compip char(20))";
	 m_NoticeTable = "create table Notice (hostip char(20),blockid char(40),status char(1))";
	 m_ReadyTaskTable = "create table ReadyTask (taskid char(40))";
	 m_ClientTable = "create table Client (ip char(20),type char(1),hostname char(64),osinfo char(64),livetime char(20),logintime char(20),gpu int,cpu int)";


	CLog::Log(LOG_LEVEL_NOMAL,"Open Database :%s OK\n",pDB);
}

CPersistencManager::~CPersistencManager(void)
{
	m_SQLite3DB.close();


}


int CPersistencManager::CreateTable(void){

	int ret = 0;
	
	//创建Task表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Task Start\n");

	if (!m_SQLite3DB.tableExists("Task")){
		m_SQLite3DB.execDML(m_TaskTable.c_str());
			
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Task Exists\n");
	}
	
	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Task End\n");
	//创建block表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Hash Start\n");
	if (!m_SQLite3DB.tableExists("Hash")){
			m_SQLite3DB.execDML(m_HashTable.c_str());
			
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Hash Exists\n");
	}

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Hash End\n");

	//创建Hash表
	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Block Start\n");
	if (!m_SQLite3DB.tableExists("Block")){
			m_SQLite3DB.execDML(m_BlockTable.c_str());
			
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Block Exists\n");
	}

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Block End\n");

	//创建ReadyTask表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :ReadyTask Start\n");
	if (!m_SQLite3DB.tableExists("ReadyTask")){
			m_SQLite3DB.execDML(m_ReadyTaskTable.c_str());
			
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :ReadyTask Exists\n");
	}

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :ReadyTask End\n");


	//创建Notice 表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Notice Start\n");
	if (!m_SQLite3DB.tableExists("Notice")){
			m_SQLite3DB.execDML(m_NoticeTable.c_str());
			
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Notice Exists\n");
	}

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Notice End\n");
    

	//创建在线客户端表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Client Start\n");
	if (!m_SQLite3DB.tableExists("Client")){
			m_SQLite3DB.execDML(m_ClientTable.c_str());
			
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Client Exists\n");
	}

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Client End\n");

	return ret;
}

int CPersistencManager::PersistTaskMap(CT_MAP task_map){

	int ret = 0;
	CT_MAP::iterator begin_task = task_map.begin();
	CT_MAP::iterator end_task = task_map.end();
	CT_MAP::iterator iter_task;
	CCrackTask *pCT = NULL;
	char insertsql[1024];

	for(iter_task = begin_task;iter_task != end_task;iter_task ++ ){

		pCT = iter_task->second;
		memset(insertsql,0,1024);
		sprintf(insertsql,"insert into task_table values ('%s','%s','%s','%s','%s','%s',%d,%d,'%s','%s',%d,%d,'%s',%f,%f,'%s',%d,%d,%d)",
			pCT->guid,pCT->algo,pCT->charset,pCT->type,pCT->special,pCT->single,pCT->startLength,pCT->endLength,pCT->m_owner,
			pCT->m_status,pCT->m_split_num,pCT->m_finish_num,pCT->m_bsuccess,pCT->m_progress,pCT->m_speed,pCT->m_start_time,
			pCT->m_running_time,pCT->m_remain_time,pCT->count);
		
		m_SQLite3DB.execDML(insertsql);

	}

	return ret;

}
int CPersistencManager::PersistHash(CT_MAP task_map){

	int ret = 0;
	CT_MAP::iterator begin_task = task_map.begin();
	CT_MAP::iterator end_task = task_map.end();
	CT_MAP::iterator task_iter;
	CCrackTask *pCT = NULL;
	CCrackHash *pCH = NULL;
	char insertsql[1024];

	int i = 0;
	int size = 0;

	for(task_iter = begin_task;task_iter != end_task;task_iter ++ ){

		pCT = task_iter->second;
		

		size = pCT->m_crackhash_list.size();

		for(i = 0 ;i < size ;i ++){

			pCH = pCT->m_crackhash_list[i];

			memset(insertsql,0,1024);
			sprintf(insertsql,"insert into Hash values ('%s',%d,'%s','%s','%s',%f)",
				pCT->guid,i,pCH->m_john,pCH->m_result,pCH->m_status,pCH->m_progress);
		
			m_SQLite3DB.execDML(insertsql);

		}

	}
	return ret;

}
int CPersistencManager::PersistBlockMap(CB_MAP block_map){

	int ret = 0;
	CB_MAP::iterator begin_block = block_map.begin();
	CB_MAP::iterator end_block = block_map.end();
	CB_MAP::iterator block_iter;
	CCrackBlock *pCB = NULL;
	char insertsql[1024];
	
	for(block_iter = begin_block;block_iter!=end_block;block_iter++){

		pCB = block_iter->second;
		memset(insertsql,0,1024);
		sprintf(insertsql,"insert into Block values ('%s','%s',%d,'%s',%f,%f,%d,'%s')",
			pCB->guid,pCB->task->guid,pCB->hash_idx,pCB->m_status,pCB->m_progress,pCB->m_speed,pCB->m_remaintime,
			pCB->m_comp_guid);

		m_SQLite3DB.execDML(insertsql);


	}


	return ret;
}

int CPersistencManager::PersistReadyTaskQueue(CT_DEQUE ready_list){
	int ret = 0;
	int i = 0;
	int size = ready_list.size();
	char insertsql[1024];
	char *p = NULL;
	

	for(i= 0;i < size;i++){
		
		p = ready_list[i];
		memset(insertsql,0,1024);
		sprintf(insertsql,"insert into ReadyTask values ('%s')",p);

		m_SQLite3DB.execDML(insertsql);


	}



	return ret;

}
int CPersistencManager::PersistClientInfo(CI_VECTOR client_list){
	int ret = 0;
	int i = 0;
	int size = client_list.size();
	CClientInfo *pCI = NULL;
	char insertsql[1024];
	int tmpgpu=0;
	int tmpcpu = 0;

	while(i < size){
		
		pCI = client_list[i];

		if (pCI->m_type == COMPUTE_TYPE_CLIENT){

			tmpgpu = ((CCompClient *)pCI)->m_gputhreads;
			tmpcpu = ((CCompClient *)pCI)->m_cputhreads;

		}


		memset(insertsql,0,1024);
		sprintf(insertsql,"insert into Client values ('%s','%s','%s','%s','%s','%s',%d,%d)",
			pCI->m_ip,pCI->m_type,pCI->m_hostname,pCI->m_osinfo,pCI->m_keeplivetime,pCI->m_logintime,
			tmpgpu,tmpcpu);

		m_SQLite3DB.execDML(insertsql);
	


		i++;
	}

	return ret;

}
int CPersistencManager::PersistNoticeMap(CCB_MAP notice_map){
	int ret = 0;
	CCB_MAP::iterator begin_notice = notice_map.begin();
	CCB_MAP::iterator end_notice = notice_map.end();
	CCB_MAP::iterator notice_iter;
	CBlockNotice *pCN = NULL;
	char insertsql[1024];


	for(notice_iter = begin_notice;notice_iter!=end_notice;notice_iter++){

		CBN_VECTOR tmpCbn = notice_iter->second;

		for(int i = 0;i < tmpCbn.size();i++){

			pCN = tmpCbn[i];

			memset(insertsql,0,1024);
			sprintf(insertsql,"insert into Notice values ('%s','%s','%s')",
				notice_iter->first.c_str(),pCN->m_guid,pCN->m_status);

			m_SQLite3DB.execDML(insertsql);
		}

	}


	return ret;

}


int CPersistencManager::LoadTaskMap(CT_MAP &task_map){
	int ret = 0;
	
	CppSQLite3Query query = m_SQLite3DB.execQuery("select * from Task order by 1;");

    while (!query.eof())
    {
        CCrackTask *pCT = new CCrackTask();
		if (!pCT){

			CLog::Log(LOG_LEVEL_ERROR,"Alloc New Task Error\n");
			return -1;
		}

		memset(pCT->guid,0,sizeof(pCT->guid));
		memcpy(pCT->guid,query.fieldValue("taskid"),strlen(query.fieldValue("taskid")));


		pCT->algo = *query.fieldValue("algo");
		pCT->charset = *query.fieldValue("charset");
		pCT->type = *query.fieldValue("type");
		pCT->special = *query.fieldValue("filetag");
		pCT->single = *query.fieldValue("single");
		pCT->startLength = query.getIntField("startlength");
		pCT->endLength = query.getIntField("endlength");
		
		memset(pCT->m_owner,0,sizeof(pCT->m_owner));
		memcpy(pCT->m_owner,query.fieldValue("owner"),strlen(query.fieldValue("owner")));

		pCT->m_status = *query.fieldValue("status");
		pCT->m_split_num = query.getIntField("splitnum");
		pCT->m_finish_num = query.getIntField("finishnum");
		pCT->m_bsuccess = (strcmp(query.fieldValue("success"),"1") == 0)? true : false;
		pCT->m_progress = query.getFloatField("progress");

		pCT->m_speed = query.getFloatField("speed");
		pCT->m_start_time = query.getIntField("starttime");
		pCT->m_running_time = query.getIntField("runtime");
		pCT->m_remain_time = query.getIntField("remaintime");

		pCT->count = query.getIntField("count");

		
		
		task_map.insert(CT_MAP::value_type(pCT->guid,pCT));



        query.nextRow();
    }




	return ret;

}

//必须在task 加载之后
int CPersistencManager::LoadHash(CT_MAP &task_map){
	int ret = 0;
	
	CT_MAP::iterator cur_iter ;
	CT_MAP::iterator end_iter  = task_map.end();
	CCrackTask *pCT = NULL;

	CppSQLite3Query query = m_SQLite3DB.execQuery("select * from Hash");

    while (!query.eof())
    {

		char *ptaskid =  (char *)query.fieldValue("taskid");
		cur_iter = task_map.find(ptaskid);
		if (cur_iter == end_iter){

			CLog::Log(LOG_LEVEL_ERROR,"Task List and Hash List is not Matched\n");
			return -1;

		}


		CCrackHash *pCH = new CCrackHash();
		if (!pCH){

			CLog::Log(LOG_LEVEL_ERROR,"Alloc New Hash Error\n");
			return -1;

		}

		
			
		pCT = cur_iter->second;

		int tmpIndex = query.getIntField("index");

		pCT->m_crackhash_list[tmpIndex] = pCH;

		memset(pCH->m_john,0,sizeof(pCH->m_john));
		memcpy(pCH->m_john,query.fieldValue("john"),strlen(query.fieldValue("john")));

		memset(pCH->m_result,0,sizeof(pCH->m_result));
		memcpy(pCH->m_result,query.fieldValue("result"),strlen(query.fieldValue("result")));

		pCH->m_status = *query.fieldValue("status");
		pCH->m_progress = query.getFloatField("progress");
	       
        query.nextRow();

    }




	return ret;

}

//必须在task 加载之后
int CPersistencManager::LoadBlockMap(CB_MAP &block_map,CT_MAP &task_map){
	int ret = 0;
	CT_MAP::iterator cur_iter ;
	CT_MAP::iterator end_iter  = task_map.end();
	CCrackTask *pCT = NULL;


	CppSQLite3Query query = m_SQLite3DB.execQuery("select * from Block");

    while (!query.eof())
    {
		char *ptaskid =(char*)query.fieldValue("taskid");
		cur_iter = task_map.find(ptaskid);
		if (cur_iter == end_iter){

			CLog::Log(LOG_LEVEL_ERROR,"Task List and Block List is not Matched\n");
			return -1;

		}

		pCT = cur_iter->second;
		CCrackBlock *pCB = new CCrackBlock();
		if (!pCB){

			CLog::Log(LOG_LEVEL_ERROR,"Alloc New Block Error\n");
			return -2;

		}

		memset(pCB->john,0,sizeof(pCB->john));

		
		memset(pCB->m_comp_guid,0,sizeof(pCB->m_comp_guid));
		memset(pCB->guid,0,sizeof(pCB->guid));
	
		memcpy(pCB->m_comp_guid,query.fieldValue("compip"),strlen(query.fieldValue("compip")));
		memcpy(pCB->guid,query.fieldValue("blockid"),strlen(query.fieldValue("blockid")));
		

		
		pCB->algo = pCT->algo;
		pCB->charset = pCT->charset;
		pCB->end = pCT->endLength;
		pCB->start = pCT->startLength;


		pCB->hash_idx = query.getIntField("index");
		memcpy(pCB->john,pCT->m_crackhash_list[pCB->hash_idx]->m_john,sizeof(pCT->m_crackhash_list[pCB->hash_idx]->m_john));


		pCB->special = pCT->special;
		pCB->task = pCT;
		pCB->m_progress = query.getFloatField("progress");
		pCB->m_speed = query.getFloatField("speed");

		pCB->m_status = *query.fieldValue("status");
		pCB->m_remaintime = query.getIntField("remaintime");

		
		cur_iter->second->m_crackblock_map.insert(CB_MAP::value_type(pCB->guid,pCB));


				
		block_map.insert(CB_MAP::value_type(pCB->guid,pCB));

	       
        query.nextRow();

    }


	return ret;
}

//必须在加载任务列表之后
int CPersistencManager::LoadReadyTaskQueue(CT_DEQUE &ready_list,CT_MAP task_map){
	int ret = 0;
	
	CT_MAP::iterator cur_iter ;
	CT_MAP::iterator end_iter  = task_map.end();


	CppSQLite3Query query = m_SQLite3DB.execQuery("select * from ReadyTask");

    while (!query.eof())
    {
		
		cur_iter = task_map.find((char *)query.fieldValue("taskid"));
		if (cur_iter == end_iter){

			CLog::Log(LOG_LEVEL_ERROR,"Task List and Ready Task List is not Matched\n");
			return -1;

		}

		ready_list.push_back(cur_iter->first);
       
        query.nextRow();

    }


	return ret;

}
int CPersistencManager::LoadClientInfo(CI_VECTOR &client_list){
	int ret = 0;
	
	int type = 0;
	CppSQLite3Query query = m_SQLite3DB.execQuery("select * from Client");

    while (!query.eof())
    {
        CClientInfo *pCI = new CClientInfo();
		if (!pCI){

			CLog::Log(LOG_LEVEL_ERROR,"Alloc New Client Info Error\n");
			return -1;
		}
		
		type = query.getIntField("type");
		if (type == COMPUTE_TYPE_CLIENT){

			((CCompClient*)pCI)->m_gputhreads = query.getIntField("gpu");
			((CCompClient*)pCI)->m_cputhreads = query.getIntField("cpu");
		}

		pCI->m_type = type;

		memset(pCI->m_ip,0,sizeof(pCI->m_ip));
		memset(pCI->m_guid,0,sizeof(pCI->m_guid));
		memset(pCI->m_hostname,0,sizeof(pCI->m_hostname));
		memset(pCI->m_osinfo,0,sizeof(pCI->m_osinfo));


		memcpy(pCI->m_ip,query.fieldValue("ip"),strlen(query.fieldValue("ip")));
		memcpy(pCI->m_hostname,query.fieldValue("hostname"),strlen(query.fieldValue("hostname")));
		memcpy(pCI->m_osinfo,query.fieldValue("osinfo"),strlen(query.fieldValue("osinfo")));


		//keeplive time and logintime 

		client_list.push_back(pCI);

        query.nextRow();
    }



	return ret;

}
int CPersistencManager::LoadNoticeMap(CCB_MAP &notice_map){
	int ret = 0;
	
	CCB_MAP::iterator cur_iter;
	CppSQLite3Query query = m_SQLite3DB.execQuery("select * from Notice");

    while (!query.eof())
    {
        CBlockNotice *pCN = new CBlockNotice();
		if (!pCN){

			CLog::Log(LOG_LEVEL_ERROR,"Alloc New Block Notice Error\n");
			return -1;
		}

		memset(pCN->m_guid,0,sizeof(pCN->m_guid));
		memcpy(pCN->m_guid,query.fieldValue("blockid"),strlen(query.fieldValue("blockid")));

		pCN->m_status = *query.fieldValue("status");

		string str;
		str = query.fieldValue("hostip");

		cur_iter = notice_map.find(str);
		if (cur_iter == notice_map.end()){

			CBN_VECTOR cbnlist;

			cbnlist.push_back(pCN);

			notice_map[str] = cbnlist;			


		}else{

			cur_iter->second.push_back(pCN);


		}

        query.nextRow();
    }

	return ret;

}





