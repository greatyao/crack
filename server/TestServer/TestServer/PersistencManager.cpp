#include "PersistencManager.h"
#include "CLog.h"
#include "CrackTask.h"
#include "CrackHash.h"
#include "CrackBlock.h"
#include "BlockNotice.h"

using std::string;
using std::vector;

CPersistencManager g_Persistence;


#define TASK_COUNT "task_count"
static inline string TASK_IDX(unsigned int id) 
{ 
	char buf[128]; 
	_snprintf(buf, sizeof(buf), "task_%08x", id); 
	return string(buf); 
}

static inline string HASH_KEY(const char* guid, int id)
{
	char buf[128]; 
	_snprintf(buf, sizeof(buf), "%s_hash_%d", guid, id); 
	return string(buf); 
}

static inline string BLOCK_IDX(const char* guid, int id)
{
	char buf[128]; 
	_snprintf(buf, sizeof(buf), "%s_item_%d", guid, id); 
	return string(buf); 
}

CPersistencManager::CPersistencManager()
{
	int ret = 0;
	m_LevelDB = NULL;
	m_useLevelDB = false;
	
	m_TaskTable = "create table Task (taskid char(40) primary key,algo short,charset short,type short,filetag short,single short,info blob,owner char(64),status short,splitnum int,finishnum int,success short,progress real,speed real,starttime int,runtime int,remaintime int,count int)";
	m_HashTable = "create table Hash (taskid char(40),index0 int,john char(260),result char(32),status short,progress real)";
	m_BlockTable = "create table Block (blockid char(40) primary key,taskid char(40), type short, index0 int, info blob, status short,progress real,speed real,remaintime int,compip char(20))";
	m_NoticeTable = "create table Notice (hostip char(20),blockid char(40),status char(1))";
	m_ReadyTaskTable = "create table ReadyTask (taskid char(40))";
	m_ClientTable = "create table Client (ip char(20),type char(1),hostname char(64),osinfo char(64),livetime char(20),logintime char(20),gpu int,cpu int)";
}


CPersistencManager::~CPersistencManager(void)
{
	if(m_useLevelDB)
	{
		if(m_LevelDB){
			m_LevelDB->~DB();
			m_LevelDB = NULL;
		}
	}
	else
		m_SQLite3DB.close();
}

bool CPersistencManager::OpenDB(const char* name, bool use_leveldb)
{
	m_useLevelDB = use_leveldb;
	
	if(m_useLevelDB)
	{
		leveldb::Options options;
		options.create_if_missing = true;
		options.write_buffer_size = 16 * 1024* 1024;
		leveldb::Status status = leveldb::DB::Open(options, string(name)+"/tmp", &m_LevelDB);
		if(!status.ok()) 
		{
			CLog::Log(LOG_LEVEL_WARNING,"Failed to Open LevelDB %s: %s\n",name, status.ToString());
			return false;
		}
		return true;
	}

	else
	{
		try{
			m_SQLite3DB.open((string(name)+".db").c_str());
		}catch(CppSQLite3Exception& ex){

			CLog::Log(LOG_LEVEL_WARNING,"Failed to Open Database %s: %s\n",name, ex.errorMessage());
			return false;
		}

		CLog::Log(LOG_LEVEL_NOMAL,"Open Database :%s OK\n", name);
			
		CreateTable();
		return true;
	}
}

int CPersistencManager::CreateTable(void){

	int ret = 0;
	
	//创建Task表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Task Start\n");
	if (!m_SQLite3DB.tableExists("Task")){
		m_SQLite3DB.execDML(m_TaskTable.c_str());
		CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Task End\n");	
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Task Exists\n");
	}
	
	//创建block表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Hash Start\n");
	if (!m_SQLite3DB.tableExists("Hash")){
			m_SQLite3DB.execDML(m_HashTable.c_str());
		CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Hash End\n");	
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Hash Exists\n");
	}

	//创建Hash表
	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Block Start\n");
	if (!m_SQLite3DB.tableExists("Block")){
			m_SQLite3DB.execDML(m_BlockTable.c_str());
		CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Block End\n");	
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Block Exists\n");
	}

	//创建ReadyTask表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :ReadyTask Start\n");
	if (!m_SQLite3DB.tableExists("ReadyTask")){
			m_SQLite3DB.execDML(m_ReadyTaskTable.c_str());
		CLog::Log(LOG_LEVEL_NOMAL,"Create Table :ReadyTask End\n");		
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :ReadyTask Exists\n");
	}


	//创建Notice 表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Notice Start\n");
	if (!m_SQLite3DB.tableExists("Notice")){
			m_SQLite3DB.execDML(m_NoticeTable.c_str());
		CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Notice End\n");		
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Notice Exists\n");
	}
    

	//创建在线客户端表

	CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Client Start\n");
	if (!m_SQLite3DB.tableExists("Client")){
			m_SQLite3DB.execDML(m_ClientTable.c_str());
		CLog::Log(LOG_LEVEL_NOMAL,"Create Table :Client End\n");
	}else{
		CLog::Log(LOG_LEVEL_NOMAL,"Table :Client Exists\n");
	}

	
	return ret;
}

//插入任务
int CPersistencManager::PersistTask(const CCrackTask *pCT, Action action)
{
	if(m_useLevelDB)
	{
		leveldb::Status s;
		leveldb::WriteOptions wo;
		leveldb::ReadOptions ro;
		string value;
		wo.sync = false;

		if(action == Insert)
		{
			int task_count = 0;
			s = m_LevelDB->Get(ro, TASK_COUNT, &value);
			if(s.ok())
				task_count = *(int*)value.data();
			task_count ++;

			s = m_LevelDB->Put(wo, TASK_COUNT, leveldb::Slice((char*)&task_count, sizeof(task_count)));

			s = m_LevelDB->Put(wo, TASK_IDX(task_count), pCT->guid);

			s = m_LevelDB->Put(wo, pCT->guid, leveldb::Slice((char*)pCT, sizeof(CCrackTask)));
		}
		else if(action == Update)
		{
			s = m_LevelDB->Put(wo, pCT->guid, leveldb::Slice((char*)pCT, sizeof(CCrackTask)));
		}
		else if(action == Delete)
		{
			s = m_LevelDB->Delete(wo, pCT->guid);
		}

		return 0;
	}

	char cmd[1024];
	
	try{
		if(action == Insert)
		{
			sprintf(cmd,"insert into Task values('%s',%d,%d,%d,%d,%d,?,'%s',%d,%d,%d,%d,%f,%f,%d,%d,%d,%d)",
				pCT->guid,pCT->algo,pCT->charset,pCT->type,pCT->special,pCT->single,pCT->m_owner,
				pCT->m_status,pCT->m_split_num,pCT->m_finish_num,pCT->m_bsuccess,pCT->m_progress,pCT->m_speed,pCT->m_start_time,
				pCT->m_running_time,pCT->m_remain_time,pCT->count);
			CppSQLite3Statement statement = m_SQLite3DB.compileStatement(cmd);
			
			unsigned char* blob = (unsigned char*)&(pCT->startLength);
			int len = pCT->filename - blob;
			statement.bind(1, blob, len);
			statement.execDML();
		}
		else if(action == Update)
		{
			sprintf(cmd,"update Task set status=%d,count=%d,splitnum=%d,success=%d where taskid='%s'",
				pCT->m_status, pCT->count, pCT->m_split_num, pCT->m_bsuccess, pCT->guid);
			m_SQLite3DB.execDML(cmd);
		}
		else if(action == Delete)
		{
			sprintf(cmd,"delete from Task where taskid='%s'", pCT->guid);
			m_SQLite3DB.execDML(cmd);
		}
	}catch(CppSQLite3Exception& ex){
		CLog::Log(LOG_LEVEL_WARNING,"Failed to [Action=%d] task %s: %s\n", action, pCT->guid, ex.errorMessage());
		return -1;
	}
	return 0;
}

//插入一个任务所有的hash表
int CPersistencManager::PersistHash(const char* guid, const CRACK_HASH_LIST& hash, Action action)
{
	if(m_useLevelDB)
	{
		leveldb::WriteOptions wo;
		wo.sync = false;
		leveldb::Status s;
		if(action == Insert || action == Update)
		{
			for(int i = 0; i < hash.size(); i++)
				s = m_LevelDB->Put(wo, HASH_KEY(guid, i), leveldb::Slice((char*)hash[i], sizeof(CCrackHash)));
		}
		else if(action == Delete)
		{
			for(int i = 0; i < hash.size(); i++)
				s = m_LevelDB->Delete(wo, HASH_KEY(guid, i));
		}

		return 0;
	}
	
	char cmd[1024];
	if(action == Insert)
	{
		int size = hash.size();
		for(int i = 0 ;i < size ;i ++){
			CCrackHash* pCH = hash[i];
			sprintf(cmd,"insert into Hash values ('%s',%d,'%s','%s', %d, %f)",
				guid, i, pCH->m_john, pCH->m_result, pCH->m_status, pCH->m_progress);
		
			try{
				m_SQLite3DB.execDML(cmd);
			}catch(CppSQLite3Exception& ex){
				CLog::Log(LOG_LEVEL_WARNING,"Failed to Insert hash %s: %s\n", guid, ex.errorMessage());
			}
		}
	}
	else if(action == Delete)
	{
		sprintf(cmd,"delete from Hash where taskid='%s'", guid);
		try{
			m_SQLite3DB.execDML(cmd);
		}catch(CppSQLite3Exception& ex){
			CLog::Log(LOG_LEVEL_WARNING,"Failed to delete hash %s: %s\n", guid, ex.errorMessage());
		}
	}

	return 0;
}

//插入一个任务的所有workitem
int CPersistencManager::PersistBlockMap(const CB_MAP& block_map, Action action){

	CB_MAP::const_iterator begin_block = block_map.begin();
	CB_MAP::const_iterator end_block = block_map.end();
	CB_MAP::const_iterator block_iter;
	
	if(m_useLevelDB)
	{
		leveldb::WriteOptions wo;
		wo.sync = false;
		leveldb::Status s;

		if(action == Insert)
		{
			std::map<string, int> mymap;
			for(block_iter = begin_block;block_iter!=end_block;block_iter++)
			{
				CCrackBlock* pCB = block_iter->second;
				char* taskguid = pCB->task->guid;

				s = m_LevelDB->Put(wo, BLOCK_IDX(taskguid, mymap[taskguid]++), pCB->guid);
					
				s = m_LevelDB->Put(wo, pCB->guid, leveldb::Slice((char*)pCB, sizeof(CCrackBlock)));
			}
		}
		else if(action == Update)
		{
			for(block_iter = begin_block;block_iter!=end_block;block_iter++)
			{
				CCrackBlock* pCB = block_iter->second;
				s = m_LevelDB->Put(wo, pCB->guid, leveldb::Slice((char*)pCB, sizeof(CCrackBlock)));
			}
		}
		else if(action == Delete)
		{
			std::map<string, int> mymap;
			for(block_iter = begin_block;block_iter!=end_block;block_iter++)
			{
				CCrackBlock* pCB = block_iter->second;
				char* taskguid = pCB->task->guid;

				s = m_LevelDB->Delete(wo, BLOCK_IDX(taskguid, mymap[taskguid]++));
				s = m_LevelDB->Delete(wo, pCB->guid);
			}
		}

		return 0;
	}

	int ret = 0;
	CCrackBlock *pCB = NULL;
	char cmd[1024];

	if(action == Insert)
	{
		for(block_iter = begin_block;block_iter!=end_block;block_iter++){

			pCB = block_iter->second;
			sprintf(cmd,"insert into Block values ('%s','%s', %d, %d, ?, %d, %f, %f, %d, '%s')",
				pCB->guid,pCB->task->guid, pCB->type, pCB->hash_idx, pCB->m_status, pCB->m_progress,
				pCB->m_speed,pCB->m_remaintime, pCB->m_comp_guid);

			try{
				CppSQLite3Statement statement = m_SQLite3DB.compileStatement(cmd);			
				unsigned char* blob = (unsigned char*)&(pCB->start);
				int len = (unsigned char*)&(pCB->task) - blob;
				statement.bind(1, blob, len);
				statement.execDML();
			} catch(CppSQLite3Exception& ex){
				CLog::Log(LOG_LEVEL_WARNING,"Failed to Insert block %s: %s\n", pCB->guid, ex.errorMessage());
			}
		}
	}
	else if(action == Delete)
	{
		if(begin_block != end_block)
		{
			pCB = begin_block->second;
			sprintf(cmd,"delete from Block where taskid='%s'", pCB->task->guid);

			try{
				m_SQLite3DB.execDML(cmd);
			}catch(CppSQLite3Exception& ex){
				CLog::Log(LOG_LEVEL_WARNING,"Failed to delete block %s: %s\n", pCB->guid, ex.errorMessage());
			}
		}
	}


	return ret;
}

int CPersistencManager::PersistReadyTaskQueue(const CT_DEQUE& ready_list){
	int ret = 0;
	int i = 0;
	int size = ready_list.size();
	char insertsql[1024];
	char *p = NULL;

	if(m_useLevelDB)
	{
		return 0;
	}
	
	for(i= 0;i < size;i++){
		
		p = ready_list[i];
		memset(insertsql,0,1024);
		sprintf(insertsql,"insert into ReadyTask values ('%s')",p);

		m_SQLite3DB.execDML(insertsql);

	}

	return ret;
}
int CPersistencManager::PersistClientInfo(const CI_VECTOR& client_list){
	int ret = 0;
	int i = 0;
	int size = client_list.size();
	CClientInfo *pCI = NULL;
	char insertsql[1024];
	int tmpgpu=0;
	int tmpcpu = 0;

	if(m_useLevelDB)
	{
		return 0;
	}

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
int CPersistencManager::PersistNoticeMap(const CCB_MAP& notice_map){
	int ret = 0;
	CCB_MAP::const_iterator begin_notice = notice_map.begin();
	CCB_MAP::const_iterator end_notice = notice_map.end();
	CCB_MAP::const_iterator notice_iter;
	CBlockNotice *pCN = NULL;

	if(m_useLevelDB)
	{
		return 0;
	}

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

	if(m_useLevelDB)
	{
		string value;
		vector<string> allguids;
		leveldb::ReadOptions ro;
		leveldb::Status s;
		int task_count = 0;

		s = m_LevelDB->Get(ro, TASK_COUNT, &value);
		if(s.ok())
			task_count = *(int*)value.data();

		for(int i = 1; i <= task_count; i++)
		{
			s = m_LevelDB->Get(ro, TASK_IDX(i), &value);
			if(s.ok())
			{
				allguids.push_back(value);
				CLog::Log(LOG_LEVEL_WARNING, "%s\n", value.c_str());
			}
		}

		for(int i = 0; i < allguids.size(); i++)
		{
			s = m_LevelDB->Get(ro, allguids[i], &value);
			if(!s.ok()) continue;

			//CCrackTask类里面有类，不能直接拷贝构造函数将内存按字节拷贝，咳!
			CCrackTask* task = (CCrackTask*)value.data();
			CCrackTask* pCT = new CCrackTask();
			memcpy(pCT, (crack_task*)task, sizeof(crack_task));
			memcpy(pCT->m_owner, task->m_owner, sizeof(task->m_owner));
			int len = (char*)&task->m_filelen - (char*)&task->m_status + sizeof(task->m_filelen);
			memcpy(&pCT->m_status, &task->m_status, len);

			task_map.insert(CT_MAP::value_type(pCT->guid,pCT));
		}

		return 0;
	}

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


		pCT->algo = query.getIntField("algo");
		pCT->charset = query.getIntField("charset");
		pCT->type = query.getIntField("type");
		pCT->special = query.getIntField("filetag");
		pCT->single = query.getIntField("single");
		int len;
		const unsigned char* info = query.getBlobField("info", len);
		memcpy(&(pCT->startLength), info, len);
		memset(pCT->m_owner,0,sizeof(pCT->m_owner));
		memcpy(pCT->m_owner,query.fieldValue("owner"),strlen(query.fieldValue("owner")));

		pCT->m_status = query.getIntField("status");
		pCT->m_split_num = query.getIntField("splitnum");
		pCT->m_finish_num = query.getIntField("finishnum");
		pCT->m_bsuccess = (strcmp(query.fieldValue("success"),"1") == 0)? true : false;
		pCT->m_progress = query.getFloatField("progress");

		pCT->m_speed = query.getFloatField("speed");
		pCT->m_start_time = query.getIntField("starttime");
		pCT->m_running_time = query.getIntField("runtime");
		pCT->m_remain_time = query.getIntField("remaintime");

		pCT->count = query.getIntField("count");

		if(pCT->type == bruteforce)
			CLog::Log(LOG_LEVEL_NOMAL, "[%d %d] %d\n", pCT->startLength, pCT->endLength, pCT->count);
		else if(pCT->type == dict)
			CLog::Log(LOG_LEVEL_NOMAL, "[dict=%d] %d\n", pCT->dict_idx, pCT->count);
		else if(pCT->type == mask)
			CLog::Log(LOG_LEVEL_NOMAL, "[mask=%s] %d\n", pCT->masks, pCT->count);
		
		task_map.insert(CT_MAP::value_type(pCT->guid,pCT));

        query.nextRow();
    }

	return ret;
}

//必须在task 加载之后
int CPersistencManager::LoadHash(CT_MAP &task_map){
	if(m_useLevelDB)
	{
		string value;
		leveldb::ReadOptions ro;
		leveldb::Status s;

		for(CT_MAP::iterator it = task_map.begin(); it != task_map.end(); it++)
		{
			CCrackTask* pCT = it->second;
			char* guid = it->first;

			pCT->m_crackhash_list.resize(pCT->count);
			for(int i = 0; i < pCT->count; i++)
			{
				s = m_LevelDB->Get(ro, HASH_KEY(guid, i), &value);
				if(!s.ok()) continue;

				CCrackHash* hash = (CCrackHash*)value.data();
				CCrackHash *pCH = new CCrackHash(*hash);
				if (!pCH){
					CLog::Log(LOG_LEVEL_ERROR,"Alloc New Hash Error\n");
					return -1;
				}

				pCT->m_crackhash_list[i] = pCH;
			}
		}
		return 0;
	}
	
	int ret = 0;
	
	for(CT_MAP::iterator it = task_map.begin(); it != task_map.end(); it++)
	{
		char* guid = it->first;
		CCrackTask *pCT = it->second;
		char cmd[1024];
		_snprintf(cmd, sizeof(cmd), "select * from Hash where taskid='%s'", guid);
		//必须将容器初始化大小
		pCT->m_crackhash_list.resize(pCT->count);

		CppSQLite3Query query = m_SQLite3DB.execQuery(cmd);

		while (!query.eof())
		{
			CCrackHash *pCH = new CCrackHash();
			if (!pCH){
				CLog::Log(LOG_LEVEL_ERROR,"Alloc New Hash Error\n");
				return -1;
			}

			int tmpIndex = query.getIntField("index0");

			pCT->m_crackhash_list[tmpIndex] = pCH;

			memset(pCH->m_john,0,sizeof(pCH->m_john));
			memcpy(pCH->m_john,query.fieldValue("john"),strlen(query.fieldValue("john")));

			memset(pCH->m_result,0,sizeof(pCH->m_result));
			memcpy(pCH->m_result,query.fieldValue("result"),strlen(query.fieldValue("result")));

			pCH->m_status = query.getIntField("status");
			pCH->m_progress = query.getFloatField("progress");
			   
			query.nextRow();
		}
	}

	return ret;
}

//必须在task 加载之后
int CPersistencManager::LoadBlockMap(CB_MAP &block_map,CT_MAP &task_map){
	
	if(m_useLevelDB)
	{
		for(CT_MAP::iterator it = task_map.begin(); it != task_map.end(); it++)
		{
			char* guid = it->first;
			CCrackTask* pCT = it->second;
			leveldb::ReadOptions ro;
			leveldb::Status s;
			string value;
			int actual_num = 0;

			for(int i = 0; i < pCT->m_split_num; i++)
			{
				s = m_LevelDB->Get(ro, BLOCK_IDX(guid, i), &value);
				if(!s.ok()) continue;

				s = m_LevelDB->Get(ro, value, &value);
				if(!s.ok()) continue;

				actual_num ++;
				CCrackBlock* item = (CCrackBlock*) value.data();
				CCrackBlock *pCB = new CCrackBlock(*item);
				if (!pCB){
					CLog::Log(LOG_LEVEL_ERROR,"Alloc New Block Error\n");
					return -2;
				}

				block_map.insert(CB_MAP::value_type(pCB->guid, pCB));
			}

			pCT->m_split_num = actual_num;
		}

		return 0;
	}
	
	int ret = 0;

	CT_MAP::iterator cur_iter ;
	CT_MAP::iterator end_iter  = task_map.end();
	CCrackTask *pCT = NULL;

	for(cur_iter = task_map.begin(); cur_iter != end_iter; cur_iter++)
	{
		char* guid = cur_iter->first;
		pCT = cur_iter->second;
		char cmd[1024];
		_snprintf(cmd, sizeof(cmd), "select * from Block where taskid='%s'", guid);

		CppSQLite3Query query = m_SQLite3DB.execQuery(cmd);

		while (!query.eof())
		{
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
			pCB->type = query.getIntField("type");
			int len;
			const unsigned char* info = query.getBlobField("info", len);
			memcpy(&(pCB->start), info, len);

			pCB->hash_idx = query.getIntField("index0");
			memcpy(pCB->john,pCT->m_crackhash_list[pCB->hash_idx]->m_john,sizeof(pCT->m_crackhash_list[pCB->hash_idx]->m_john));

			pCB->special = pCT->special;
			pCB->task = pCT;
			pCB->m_progress = query.getFloatField("progress");
			pCB->m_speed = query.getFloatField("speed");

			pCB->m_status = query.getIntField("status");
			pCB->m_remaintime = query.getIntField("remaintime");

			
			cur_iter->second->m_crackblock_map.insert(CB_MAP::value_type(pCB->guid,pCB));
					
			block_map.insert(CB_MAP::value_type(pCB->guid,pCB));
		       
			query.nextRow();
		}
	}

	return ret;
}

//必须在加载任务列表之后
int CPersistencManager::LoadReadyTaskQueue(CT_DEQUE &ready_list,CT_MAP task_map){
	if(m_useLevelDB)
	{
		return 0;
	}
	
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
	if(m_useLevelDB)
	{
		return 0;
	}
	
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
	if(m_useLevelDB)
	{
		return 0;
	}
	
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