#include "StdAfx.h"
#include "PackManager.h"
#include "SocketClient.h"
#include "CLog.h"

CPackManager g_packmanager;

CPackManager::CPackManager(void)
{
	CLog::InitLogSystem(LOG_TO_FILE,TRUE,"ControlClient.log");

	m_connected = 0;
	StartClient();
}

CPackManager::~CPackManager(void)
{
	StopClient();
}


int CPackManager::StartClient(void)
{
	if(m_connected)//已经连接服务器
	{
		return 0;
	}	

	char ip[20]="192.168.18.117";
	//直接读配置
	char ini_file[MAX_PATH]={0};
	GetModuleFileNameA(NULL,ini_file,MAX_PATH);
	strcat(ini_file,".ini");

	GetPrivateProfileString("config","ip","127.0.0.1",ip,MAX_PATH-1,ini_file);

	if( m_sockclient.Init(ip,6010)!=0 )
	{
		AfxMessageBox("连接服务器失败");
	}
	m_connected = 1;

	//发送登陆包
	client_login_req req={0};
	SYSTEM_INFO s_info;
	GetSystemInfo(&s_info);


	struct sockaddr_in addr;
	socklen_t len2 = sizeof(addr);
	getsockname(m_sockclient.m_clientsocket, (sockaddr *)&addr, &len2);
	strncpy(req.m_ip, inet_ntoa(addr.sin_addr), sizeof(req.m_ip));//ip
	req.m_port = ntohs(addr.sin_port);//port
	//操作系统信息
	OSVERSIONINFOA ov_ver_info={0};
	ov_ver_info.dwOSVersionInfoSize = sizeof(ov_ver_info);
	GetVersionExA(&ov_ver_info);
	wsprintfA(req.m_osinfo,"%d.%d.%d %s",ov_ver_info.dwMajorVersion,ov_ver_info.dwMinorVersion,ov_ver_info.dwBuildNumber,ov_ver_info.szCSDVersion);
	
	// 主机名
	gethostname(req.m_hostinfo, sizeof(req.m_hostinfo));
	//char m_hostinfo[64];	//机器名
	req.m_type = CONTROL_TYPE_CLIENT;			//客户端类型,control , compute
	req.m_gputhreads = -1;		//GPU数目
	req.m_cputhreads = s_info.dwNumberOfProcessors;
	req.m_clientsock = -1;

	DoLoginPack(req);

	//心跳线程使用
	m_bThreadHeartBeatRunning = 0;
	StartHeartBeat();//心跳

	return 1;
}

int CPackManager::StopClient(void)
{
	if(m_connected==0)//服务器未连接
	{
		return 0;
	}

	StopHeartBeat();//心跳
	m_sockclient.Finish();
	m_connected = 0;
	return 1;
}

int CPackManager::CheckConnect(void)
{
	if(m_connected==0)
	{
		//尝试连接
		return StartClient();
	}

	return 1;
}


int CPackManager::DoLoginPack(client_login_req req){
	
	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send TOKEN LOGIN ...\n");
	ret = m_sockclient.Write(TOKEN_LOGIN,0,&req,sizeof(client_login_req));
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send TOKEN LOGIN Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv TOKEN LOGIN Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv TOKEN LOGIN OK\n");
	return status;
}
int CPackManager::DoKeeplivePack(){

	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send HeartBeat Req...\n");
	ret = m_sockclient.Write(TOKEN_HEARTBEAT,0,NULL,0);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Heartbeat Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv HeartBeat res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv HeartBeat Res OK\n");
	return status;


}
	
int CPackManager::DoTaskUploadPack(crack_task req,task_upload_res *res){

	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send task Upload Req ...\n");
	ret = m_sockclient.Write(CMD_TASK_UPLOAD,0,&req,sizeof(crack_task));
	if (ret <0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Task Upload req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Task Upload res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Task Upload res OK\n");
	if (status == 0)
		memcpy(res,recbuf,sizeof(task_upload_res));

	return status;


}
/*
CMD_TASK_START,		//开始任务
	CMD_TASK_STOP,		//停止任务
	CMD_TASK_DELETE,	//删除任务
	CMD_TASK_PAUSE,		//暂停任务
	CMD_TASK_RESULT,	//取得解密信息（解密状态）
*/

int CPackManager::GenTaskStartPack(task_start_req req,task_status_res *res){

	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Start Req...\n");
	ret = m_sockclient.Write(CMD_TASK_START,0,&req,sizeof(task_start_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Task Start Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Task Start res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Task Start Res OK\n");

	if (status == 0){
		memcpy(res,recbuf,sizeof(task_status_res));

	}
	return status;

}


int CPackManager::GenTaskStopPack(task_stop_req req,task_status_res *res){

	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Stop Req ...\n");
	ret = m_sockclient.Write(CMD_TASK_STOP,0,&req,sizeof(task_stop_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Task Stop req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Task Stop res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Task Stop res OK\n");
	if (status == 0){
		memcpy(res,recbuf,sizeof(task_status_res));

	}
	return ret;

}
int CPackManager::GenTaskPausePack(task_pause_req req,task_status_res *res){

	CheckConnect();

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Pause req...\n");
	ret = m_sockclient.Write(CMD_TASK_PAUSE,0,&req,sizeof(task_pause_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Taks Pause Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Task Pause Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Task Pause Res OK\n");
	if (status == 0){
		memcpy(res,recbuf,sizeof(task_status_res));

	}
	return ret;

}
int CPackManager::GenTaskDeletePackt(task_delete_req req,task_status_res *res){

	CheckConnect();

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Delete Req ...\n");
	ret = m_sockclient.Write(CMD_TASK_DELETE,0,&req,sizeof(task_delete_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Task Delete Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Task Delte Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Task Delete Res OK\n");
	if (status == 0){
		memcpy(res,recbuf,sizeof(task_status_res));

	}
	return ret;

}

int CPackManager::GenTaskResultPack(task_result_req req,task_status_res *res){

	CheckConnect();

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Get A Task Result Req...\n");
	ret = m_sockclient.Write(CMD_TASK_RESULT,0,&req,sizeof(task_result_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send TOKEN Get A Task Result ReqError\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Get A Task Result Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Get A Task Result Res OK\n");
	if (status == 0){
		memcpy(res,recbuf,sizeof(task_status_res));

	}
	return ret;

}
/*
CMD_REFRESH_STATUS,	//取得任务的进度和状态等信息
	CMD_GET_CLIENT_LIST,//返回在线计算机信息的列表
	*/
int CPackManager::GenTaskStatusPack(task_status_info **res){

	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	void *pres = NULL;

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Get Tasks Status Req ...\n");
	ret = m_sockclient.Write(CMD_REFRESH_STATUS,0,NULL,0);
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Get Tasks Status Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Tasks Status Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Tasks Status Res OK\n");
	if ((status == 0 ) && (ret > 0)){

		pres = malloc(ret);
		if (!pres){
			
			return -1;
		}
		
		memcpy(pres,recbuf,ret);

		*res =(task_status_info*)pres;

	}
	return ret;


}
int CPackManager::GenClientStatusPack(compute_node_info **res){

	CheckConnect();
int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	void *pres =NULL;

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Client Status Req ...\n");
	ret = m_sockclient.Write(CMD_GET_CLIENT_LIST,0,NULL,0);
	if (ret != 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Client Status Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Client Status Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Client Status Res OK\n");
	if ((status == 0 ) && (ret > 0)){

		pres = malloc(ret);
		if (!pres){
			
			return -1;
		}
		
		memcpy(pres,recbuf,ret);

		*res =(compute_node_info*)pres;

	}

	return ret;


}


int CPackManager::GenFileUploadPack(file_upload_req req,file_upload_res *res){

	CheckConnect();
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send File Upload Req ...\n");
	ret = m_sockclient.Write(CMD_UPLOAD_FILE,0,&req,sizeof(file_upload_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send File Upload Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv File Upload Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv File Upload Res OK\n");
	if (status == 0){

		memcpy(res,recbuf,sizeof(file_upload_res));
		memset(m_cur_upload_guid,0,40);
		memcpy(m_cur_upload_guid,res->guid,40);
	}
	return ret;


}
int CPackManager::GenFileUploadStart(file_upload_end_res *res){

	CheckConnect();
	FILE *fp = NULL;
	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	file_upload_start_res start_res;	
	file_upload_start_req start_req;
	file_upload_end_res end_res;

	unsigned int filelen = 0;

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Start File Upload Req ...\n");
	
	fp = fopen((char *)m_cur_local_file,"rb");
	if (!fp){
		

		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s Error\n",m_cur_local_file);
		return -1;

	}

	fseek(fp,0L,SEEK_END);

	filelen = ftell(fp);

	fseek(fp,0L,SEEK_SET);

	m_cur_upload_file_len = filelen;
	memset(&start_req,0,sizeof(file_upload_start_req));
	start_req.len = filelen;
	start_req.offset = 0;
	memcpy(start_req.guid,m_cur_upload_guid,40);
	start_req.f = fp;



	ret = m_sockclient.Write(CMD_START_UPLOAD,0,&start_req,sizeof(file_upload_start_req));
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Send Start File Upload Req Error\n");
		fclose(fp);
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Start File Upload Res Error\n");
		fclose(fp);
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Start File Upload Res OK\n");
	
	if (status < 0){
		
		fclose(fp);
		return -1;

	}else {

		memcpy(&start_res,recbuf,sizeof(file_upload_start_res));

		m_cur_server_file = start_res.f;
		
	}

	
	//transport the file content
	int readLen = 0;

	while(!feof(fp)){
		memset(recbuf,0,sizeof(recbuf));
		readLen = fread(recbuf,1,1024,fp);
		if (readLen < 0 ){
			CLog::Log(LOG_LEVEL_WARNING,"read file Error\n");
			fclose(fp);
			return -1;
		}
		//send file buffer 
		ret = m_sockclient.WriteNoCompress(CMD_START_UPLOAD,0,recbuf,readLen);
		if(ret < 0 ){

			CLog::Log(LOG_LEVEL_WARNING,"Send file buffer error\n");
			break;
		}

		CLog::Log(LOG_LEVEL_WARNING,"Send file buffer %d vs %d ok\n",ret,m_cur_upload_file_len);
		//
	}



	//recve the upload res
	memset(recbuf,0,1024*4);

	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv End File Upload Res Error\n");
		fclose(fp);
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv End File Upload Res OK\n");
	
	fclose(fp);
	if (status == 0){
	
		memcpy(res,recbuf,sizeof(file_upload_end_res));

	}
	return ret;

}




/*************************************************************************
心跳包线程处理
必须初始化 m_bThreadHeartBeatRunning 为 0
然后可以调用 StartHeartBeat 和 StopHeartBeat 开始和结束线程
*************************************************************************/
#pragma comment(lib,"pthreadVC2.lib")

/***********************************************
心跳包线程
定义间隔时间60秒
***********************************************/
void *CPackManager::ThreadHeartBeat(void *par)
{	
	CPackManager *p = (CPackManager*)par;
	
	while(p->m_bThreadHeartBeatStop!=TRUE)
	{
		p->DoKeeplivePack();
		p->StartSleep(p->m_hStopSleep,120*1000);//等120秒
	}
	return 0;
};	

/***********************************************
启动心跳包线程
***********************************************/
void CPackManager::StartHeartBeat(void)
{	
	if(m_bThreadHeartBeatRunning==TRUE)
	{
		CLog::Log(LOG_LEVEL_WARNING,"线程运行中，不需要再创建\n");
		return;
	}
	
	m_bThreadHeartBeatStop = FALSE;
	m_bThreadHeartBeatRunning = TRUE;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	pthread_mutex_t running_mutex;
	pthread_cond_t keeprunning_cv;
	pthread_mutex_init(&running_mutex, NULL);
	pthread_cond_init(&keeprunning_cv, NULL);
	
	m_hStopSleep = CreateSleep();
	
	int returnValue = pthread_create( &m_ThreadHeartBeat, &attr, ThreadHeartBeat, (void *)this);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"心跳线程失败，错误代码: %d\n", returnValue);
		m_bThreadHeartBeatStop = TRUE;
		m_bThreadHeartBeatRunning = FALSE;
		this->StopSleep(m_hStopSleep);
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"心跳线程创建成功\n");
		m_bThreadHeartBeatStop = FALSE;
		m_bThreadHeartBeatRunning = TRUE;
	}
}

/***********************************************
结束心跳包线程
***********************************************/
void CPackManager::StopHeartBeat(void)
{
	if(m_bThreadHeartBeatRunning==FALSE)
	{
		CLog::Log(LOG_LEVEL_NOMAL,"线程没有在运行\n");
		return;
	}

	m_bThreadHeartBeatStop = TRUE;
	this->StopSleep(m_hStopSleep);
	int returnValue = pthread_join(m_ThreadHeartBeat, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"线程退出失败，错误代码: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"线程成功退出\n");
	}
	m_bThreadHeartBeatRunning = FALSE;
}


/***********************************************
可中断sleep
***********************************************/
#pragma comment(lib,"Rpcrt4.lib")
HANDLE CPackManager::CreateSleep(void)
{
	char eventName[MAX_PATH]={0};
	UUID uuid;
	UuidCreate(&uuid);
	UuidToStringA(&uuid,(RPC_CSTR*)&eventName);
	
	HANDLE hEvent = CreateEventA(NULL,TRUE,FALSE,eventName);
	return hEvent;
}

void CPackManager::StartSleep(HANDLE hHandle,unsigned long dwMilliseconds)
{
	WaitForSingleObject(hHandle, dwMilliseconds);
}

void CPackManager::StopSleep(HANDLE hHandle)
{
	SetEvent(hHandle);
	CloseHandle(hHandle);
}


/*************************************************************************
网络数据收发线程
必须初始化 m_bThreadSendRunning 为 0
然后可以调用 StartSend 和 StopSend 开始和结束线程
*************************************************************************/


/***********************************************
收发数据线程
***********************************************/
void *CPackManager::ThreadSend(void *par)
{	
	CPackManager *p = (CPackManager*)par;
	
	while(p->m_bThreadSendStop!=TRUE)
	{
		if(p->m_bToSend)//有数据发送
		{
			//发送
			//p->m_sockclient.mysend(p->m_pSend,p->m_lenSend,0);
			//mysend不是对外读写的接口
			free(p->m_pSend); p->m_pSend=0;
			p->m_bToSend = 0;

			//接收
			//unsigned char *m_pRecv;//接收到数据的指针
			//int m_bRecved;//接收到数据
			//int m_lenRecv;//

		}

		Sleep(111);
	}
	return 0;
};	

/***********************************************
启动心跳包线程
***********************************************/
void CPackManager::StartSend(void)
{	
	if(m_bThreadSendRunning==TRUE)
	{
		CLog::Log(LOG_LEVEL_WARNING,"线程运行中，不需要再创建\n");
		return;
	}

	m_pSend  = 0;
	m_bToSend= 0;
	m_lenSend= 0;

	m_pRecv  = 0;
	m_bRecved= 0;
	m_lenRecv= 0;
	
	m_bThreadSendStop = FALSE;
	m_bThreadSendRunning = TRUE;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	pthread_mutex_t running_mutex;
	pthread_cond_t keeprunning_cv;
	pthread_mutex_init(&running_mutex, NULL);
	pthread_cond_init(&keeprunning_cv, NULL);
	
	int returnValue = pthread_create( &m_ThreadSend, &attr, ThreadSend, (void *)this);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"网络线程失败，错误代码: %d\n", returnValue);
		m_bThreadSendStop = TRUE;
		m_bThreadSendRunning = FALSE;
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"线程创建成功\n");
		m_bThreadSendStop = FALSE;
		m_bThreadSendRunning = TRUE;
	}
}

/***********************************************
结束网络数据收发线程
***********************************************/
void CPackManager::StopSend(void)
{
	if(m_bThreadSendRunning==FALSE)
	{
		CLog::Log(LOG_LEVEL_NOMAL,"线程没有在运行\n");
		return;
	}

	m_bThreadSendStop = TRUE;
	int returnValue = pthread_join(m_ThreadSend, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"线程退出失败，错误代码: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"线程成功退出\n");
	}
	m_bThreadSendRunning = FALSE;

	if(m_pSend)
	{
		free(m_pSend);
	}
	if(m_pRecv)
	{
		free(m_pRecv);
	}
}

/***********************************************
网络收发数据外部接口
***********************************************/
void *CPackManager::SendDataViaThread(void *)
{
	return 0;
}

