#include "StdAfx.h"
#include "PackManager.h"
#include "SocketClient.h"
#include "CLog.h"
#include "err.h"
#include <time.h>

CPackManager g_packmanager;

CPackManager::CPackManager(void)
{
	ReadConfigure();

	InitLockSocket();
	m_connected = 0;
	m_bThreadHeartBeatRunning = 0;
	StartClient();
	StartHeartBeat();
}

void CPackManager::ReadConfigure(void)
{
	char buffer[20];
	//ֱ�Ӷ�����
	char ini_file[MAX_PATH]={0};
	GetModuleFileNameA(NULL,ini_file,MAX_PATH);
	strcat(ini_file,".ini");

	GetPrivateProfileString("config","user","",m_user,sizeof(m_user),ini_file);
	GetPrivateProfileString("config","password","",m_passwd,sizeof(m_passwd),ini_file);
	GetPrivateProfileString("config","ip","192.168.18.115",m_sIp,MAX_PATH-1,ini_file);
	
	GetPrivateProfileString("config","port","6010",buffer,20,ini_file);
	m_nPort = atoi(buffer);
	
	GetPrivateProfileString("config","logtype","1",buffer,20,ini_file);
	m_logtype = atoi(buffer);

	CLog::InitLogSystem(m_logtype,TRUE,"ControlClient.log");
}

void CPackManager::ReadConfigure(char *ip,int *port,char*user,char* passwd, int* log)
{
	sprintf(ip,"%s",m_sIp);
	port[0] = m_nPort;
	sprintf(user,"%s",m_user);
	sprintf(passwd,"%s",m_passwd);
	*log = m_logtype;
}

int CPackManager::SetConfigure(const char *ip,int port, const char*user, const char* passwd, int log)
{
	char buffer[20];
	//ֱ�Ӷ�����
	char ini_file[MAX_PATH]={0};
	GetModuleFileNameA(NULL,ini_file,MAX_PATH);
	strcat(ini_file,".ini");

	WritePrivateProfileString("config","user",user,ini_file);
	WritePrivateProfileString("config","password",passwd,ini_file);
	WritePrivateProfileString("config","ip",ip,ini_file);
	
	sprintf(buffer,"%d",port);
	WritePrivateProfileString("config","port",buffer,ini_file);

	sprintf(buffer,"%d", log);
	WritePrivateProfileString("config","logtype",buffer,ini_file);
	
	bool login = false;
	if(strcmp(m_user, user) != 0 || strcmp(m_passwd, passwd) != 0 || strcmp(m_sIp, ip) || m_nPort != port)
		login = true;

	if(m_logtype != log)
	{
		CLog::ReleaseLogSystem();
		CLog::InitLogSystem(log, TRUE,"ControlClient.log");
	}

	m_nPort = port;
	m_logtype = log;
	_snprintf(m_sIp,sizeof(m_sIp),"%s",ip);
	_snprintf(m_user,sizeof(m_user),"%s",user);
	_snprintf(m_passwd,sizeof(m_passwd), "%s",passwd);

	if(login)
	{
		m_sockclient.Finish();
		m_connected = 0;
	}
	
	return 1;
}

CPackManager::~CPackManager(void)
{
	StopClient();
	DelLockSocket();
}

#import "progid:WbemScripting.SWbemLocator" named_guids
void GetOSName(char *pname)
{
	static char m_sName[MAX_PATH]={0};
	if(m_sName[0]!=0)
	{
		strcpy(pname,m_sName);
		return ;
	}


	CoInitialize(NULL);
	try
	{
	WbemScripting::ISWbemLocatorPtr locator;
	locator.CreateInstance(WbemScripting::CLSID_SWbemLocator);
	if (locator != NULL)
	{
		WbemScripting::ISWbemServicesPtr services = locator->ConnectServer(".","root\\cimv2","","","","",0,NULL);
		WbemScripting::ISWbemObjectSetPtr objects = services->ExecQuery("Select * from Win32_OperatingSystem","WQL",0x10,NULL);
		IEnumVARIANTPtr obj_enum = objects->Get_NewEnum();	
		ULONG fetched;
		VARIANT var;
		while (obj_enum->Next(1,&var,&fetched) == S_OK)
		{
			WbemScripting::ISWbemObjectPtr object = var;
			WbemScripting::ISWbemPropertySetPtr properties = object->Properties_;
			WbemScripting::ISWbemPropertyPtr prop = properties->Item("Name",0);
			_variant_t value = prop->GetValue();

			sprintf(m_sName,"%s\n",(const char*)_bstr_t(prop->GetValue()));
			for(int i=0; i<MAX_PATH; i++)
			{
				pname[i] = m_sName[i];
				if(pname[i]=='|')
				{
					pname[i] = 0;
					break;
				}
				
				if(pname[i]=='0')
				{
					break;
				}
			}
			break;
		}
	}
	}
	catch (_com_error err)
	{
	}
	CoUninitialize();
}


void CPackManager::GetErrMsg(short status,char *msg){

	switch(status){

		case DELET_TASK_ERROR:

			strcpy(msg,"ɾ����������ʧ��");
			break;
		case LOAD_FILE_ERROR:
			
			strcpy(msg,"�ļ���ʽ��ƥ��");
			break;
		case SPLIT_BLOCK_ERROR:

			strcpy(msg,"�з�HASH����");
			break;
		default:
			strcpy(msg,"δ֪����");
			break;

	}
}

int CPackManager::StartClient(void)
{
	if(m_connected)//�Ѿ����ӷ�����������������
	{
		return 0;
	}

	m_connected = 1;
	if( m_sockclient.Init(m_sIp,m_nPort)!=0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"���ӷ�����(%s:%d)ʧ��\n",m_sIp,m_nPort);
		m_connected = 0;
		return 0;
	}
	m_connected = 2;

	//���͵�½��
	client_login_req req={0};
	SYSTEM_INFO s_info;
	GetSystemInfo(&s_info);

	struct sockaddr_in addr;
	socklen_t len2 = sizeof(addr);
	getsockname(m_sockclient.m_clientsocket, (sockaddr *)&addr, &len2);
	strncpy(req.m_ip, inet_ntoa(addr.sin_addr), sizeof(req.m_ip));//ip
	req.m_port = ntohs(addr.sin_port);//port
	//����ϵͳ��Ϣ
	OSVERSIONINFOA ov_ver_info={0};
	ov_ver_info.dwOSVersionInfoSize = sizeof(ov_ver_info);
	GetVersionExA(&ov_ver_info);
	//wsprintfA(req.m_osinfo,"%d.%d.%d %s",ov_ver_info.dwMajorVersion,ov_ver_info.dwMinorVersion,ov_ver_info.dwBuildNumber,ov_ver_info.szCSDVersion);
	GetOSName(req.m_osinfo);

	// ������
	gethostname(req.m_hostinfo, sizeof(req.m_hostinfo));
	//char m_hostinfo[64];	//������
	req.m_type = CONTROL_TYPE_CLIENT;			//�ͻ�������,control , compute
	req.m_gputhreads = -1;		//GPU��Ŀ
	req.m_cputhreads = s_info.dwNumberOfProcessors;
	req.m_clientsock = -1;
	strncpy(req.m_user, m_user, sizeof(m_user));
	strncpy(req.m_password, m_passwd, sizeof(m_passwd));
	
	DoLoginPack(req);

	return 1;
}

int CPackManager::StopClient(void)
{
	if(m_connected==0)//������δ����
	{
		return 0;
	}

	StopHeartBeat();//����
	m_sockclient.Finish();
	m_connected = 0;
	return 1;
}


int CPackManager::DoLoginPack(client_login_req req){
	
	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send TOKEN LOGIN ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_LOGIN,0,&req,sizeof(client_login_req));
	if (ret < 0){
		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send TOKEN LOGIN Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv TOKEN LOGIN Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv TOKEN LOGIN OK\n");
	return status;
}

int CPackManager::DoKeeplivePack(){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	LockSocket();
	CLog::Log(LOG_LEVEL_WARNING,"Send HeartBeat Req...\n");
	ret = m_sockclient.Write(CMD_HEARTBEAT,0,NULL,0);
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Heartbeat Req Error %d\n", ret);
		
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv HeartBeat res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv HeartBeat Res OK\n");
	return status;
}
	
int CPackManager::DoTaskUploadPack(crack_task req,task_upload_res *res){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send task Upload Req [%d,%s]...\n", req.algo, req.filename);
	LockSocket();
	ret = m_sockclient.Write(CMD_TASK_UPLOAD,0,&req,sizeof(crack_task));
	if (ret <0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Task Upload req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
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
CMD_TASK_START,		//��ʼ����
	CMD_TASK_STOP,		//ֹͣ����
	CMD_TASK_DELETE,	//ɾ������
	CMD_TASK_PAUSE,		//��ͣ����
	CMD_TASK_RESULT,	//ȡ�ý�����Ϣ������״̬��
*/

int CPackManager::GenTaskStartPack(task_start_req req,task_status_res *res){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Start Req...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_TASK_START,0,&req,sizeof(task_start_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Task Start Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
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

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Stop Req ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_TASK_STOP,0,&req,sizeof(task_stop_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Task Stop req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
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

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Pause req...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_TASK_PAUSE,0,&req,sizeof(task_pause_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Taks Pause Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
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

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Task Delete Req ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_TASK_DELETE,0,&req,sizeof(task_delete_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Task Delete Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
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


int CPackManager::GenTaskResultPack(task_result_req req,task_result_info **res){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	struct task_result_info *pres = NULL;

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Get Task Result Req ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_TASK_RESULT,0,&req,sizeof(task_result_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Get Task Result Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Get Task Result Res Error\n");
		return ret;

	}
	CLog::Log(LOG_LEVEL_WARNING,"Recv Get Task Result Res OK\n");
	if (status == 0){
		//memcpy(res,recbuf,sizeof(task_status_res));
		pres = (task_result_info *)_malloc(ret);
		if (!pres){

			CLog::Log(LOG_LEVEL_WARNING,"Malloc Get Task Result Res Error\n");
			ret = -1;
			
		}else{



			memset(pres,0,ret);
			memcpy(pres,recbuf,ret);

			*res = pres;

		}

	}
	return ret;
}

/*
CMD_REFRESH_STATUS,	//ȡ������Ľ��Ⱥ�״̬����Ϣ
	CMD_GET_CLIENT_LIST,//�������߼������Ϣ���б�
	*/
int CPackManager::GenTaskStatusPack(task_status_info **res){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	void *pres = NULL;

	memset(recbuf,0,1024*4);

	//CLog::Log(LOG_LEVEL_WARNING,"Send Get Tasks Status Req ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_REFRESH_STATUS,0,NULL,0);
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Get Tasks Status Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv Tasks Status Res Error\n");
		return ret;

	}
	//CLog::Log(LOG_LEVEL_WARNING,"Recv Tasks Status Res OK\n");
	if ((status == 0 ) && (ret > 0)){

		pres = _malloc(ret);
		if (!pres){
			
			return -1;
		}
		
		memcpy(pres,recbuf,ret);

		*res =(task_status_info*)pres;

	}
	return ret;


}
int CPackManager::GenClientStatusPack(compute_node_info **res){
	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	void *pres =NULL;

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_WARNING,"Send Client Status Req ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_GET_CLIENT_LIST,0,NULL,0);
	if (ret != 0){
		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send Client Status Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
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

//�޸����ļ��ϴ�Э��,��ع�������
// client------->server : upload request
// server------->client : upload response
// client------->server : upload start request
// server------->client : upload start response
// client------->server : upload file infomation
//  ......
// client------->server : upload end request
// server------->client : upload end response



//�����ļ��ϴ�
int CPackManager::GenNewFileUploadPack(file_upload_req req,file_upload_res *res){
		
	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];

	memset(recbuf,0,1024*4);

	CLog::Log(LOG_LEVEL_NOMAL,"Send File Upload Req...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_UPLOAD_FILE,0,&req,sizeof(file_upload_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send File Upload Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	UnLockSocket();
	if (ret < 0){
		CLog::Log(LOG_LEVEL_WARNING,"Recv File Upload Res Error\n");
		return ret;

	}
	if (status == 0){
		memcpy(res,recbuf,sizeof(file_upload_res));
		memset(m_cur_upload_guid,0,40);
		memcpy(m_cur_upload_guid,res->guid,40);
		this->m_cur_server_file = res->f;
		CLog::Log(LOG_LEVEL_NOTICE,"Recv File Upload Res OK status=%d guid=%s\n", status, res->guid);
	}
	else
		CLog::Log(LOG_LEVEL_WARNING,"Recv File Upload Res OK status=%d\n", status);

	return ret;
}


	//�����ļ��ϴ���ʼ
int CPackManager::GenNewFileUploadStartPack(file_upload_start_res *res){
	
	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	file_upload_start_req req;
	int filelen = 0;
	FILE *fp = NULL;

	fp = fopen((char *)m_cur_local_file,"rb");
	if (!fp){
		CLog::Log(LOG_LEVEL_WARNING,"fopen file %s Error\n",m_cur_local_file);
		return -1;
	}

	fseek(fp,0L,SEEK_END);

	filelen = ftell(fp);

	fseek(fp,0L,SEEK_SET);

	m_cur_upload_file_len = filelen;
	//this->m_cur_local_file = fp;
	this->m_file_desc = fp;

	
	memset(&req,0,sizeof(file_upload_start_req));
	req.f = this->m_cur_server_file;
	memcpy(req.guid,this->m_cur_upload_guid,40);
	req.len = filelen;

	CLog::Log(LOG_LEVEL_NOMAL,"Send File Upload Start Req ...\n");
	CLog::Log(LOG_LEVEL_NOMAL,"guid=%s fd=%p len=%d\n", m_cur_upload_guid, fp, filelen);
	LockSocket();
	ret = m_sockclient.Write(CMD_START_UPLOAD,0,&req,sizeof(file_upload_start_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send File Upload Start Req Error\n");
		return ret;
	}
	
	UnLockSocket();	
	return ret;
}

	//�ļ�����
int CPackManager::GenNewFileUploadingPack(){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;

	unsigned int readLen = 0;
	unsigned char sendbuf[1024*4];
	unsigned int totalLen = 0;;
	FILE *fp = NULL;
	int step = 1;
	
	//file read 
	
	fp = (FILE *)this->m_file_desc;
	CLog::Log(LOG_LEVEL_NOMAL, "Send file content fd=%p ...\n",fp);
	
	LockSocket();
	while(!feof(fp)){
		memset(sendbuf,0,sizeof(sendbuf));
		readLen = fread(sendbuf,1,sizeof(sendbuf)/step,fp);
		if (readLen < 0 ){
			UnLockSocket();
			CLog::Log(LOG_LEVEL_WARNING,"read file Error\n");
			return -1;
		}
		else if (readLen == 0 ){
			break;
		}

		//send file buffer 
		//CLog::Log(LOG_LEVEL_WARNING,"read %d bytes and send ...\n", readLen);
		clock_t t1 = clock();
		ret = m_sockclient.WriteNoCompress(CMD_FILE_CONTENT,0,sendbuf,readLen);
		if(ret < 0 ){

			CLog::Log(LOG_LEVEL_WARNING,"Send file buffer error %d\n", ret);
			break;
		}
		double t = 1.0*(clock() - t1)/CLOCKS_PER_SEC;
		if(t < 1e-3)
			t = 0.0005;
		int speed=readLen/t;
		if(speed >= 2000000)
			step = 1;
		else if(speed >= 800000)
			step = 2;
		else if(speed >= 100000)
			step = 4;
		else if(speed >= 40000)
			step = 8;
		else 
			step = 16;
		totalLen += readLen;
		CLog::Log(LOG_LEVEL_DEBUG,"Send file buffer %d/%d [speed=%d]\n",totalLen,m_cur_upload_file_len, speed);
		//
	}
	UnLockSocket();
	CLog::Log(LOG_LEVEL_NOMAL,"Send file OK\n");
	fclose(fp);
}

	//�����ļ��ϴ�����
int CPackManager::GenNewFileUploadEndPack(file_upload_end_res *res){

	if(m_connected != 2)
		return ERR_CONNECTIONLOST;

	int ret = 0;
	unsigned char cmd;
	short status;
	unsigned char recbuf[1024*4];
	file_upload_end_req req;

	memset(recbuf,0,1024*4);

	memset(&req,0,sizeof(file_upload_end_req));
	req.f = this->m_cur_server_file;
	memcpy(req.guid,this->m_cur_upload_guid,40);
	req.len = this->m_cur_upload_file_len;
	req.offset = 0;

	CLog::Log(LOG_LEVEL_NOMAL, "Send File Upload End Req ...\n");
	LockSocket();
	ret = m_sockclient.Write(CMD_END_UPLOAD,0,&req,sizeof(file_upload_end_req));
	if (ret < 0){

		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Send File Upload End Req Error\n");
		return ret;
	}
	
	ret = m_sockclient.Read(&cmd,&status,recbuf,1024*4);
	if (ret < 0){
		UnLockSocket();
		CLog::Log(LOG_LEVEL_WARNING,"Recv File Upload End Res Error\n");
		return ret;

	}
	UnLockSocket();
	CLog::Log(LOG_LEVEL_NOMAL,"Recv File Upload End Res OK\n");
	if (status == 0){

		memcpy(res,recbuf,sizeof(file_upload_end_res));
		memset(m_cur_upload_guid,0,40);
		memcpy(m_cur_upload_guid,res->guid,40);
		this->m_cur_server_file = res->f;
		ret = 0;
	}else{
		
		res->offset = status;

		ret = -20;

	}
	return ret;
}




/*************************************************************************
�������̴߳���
�����ʼ�� m_bThreadHeartBeatRunning Ϊ 0
Ȼ����Ե��� StartHeartBeat �� StopHeartBeat ��ʼ�ͽ����߳�
*************************************************************************/
#pragma comment(lib,"pthreadVC2.lib")

/***********************************************
�������߳�
������ʱ��60��
***********************************************/
void *CPackManager::ThreadHeartBeat(void *par)
{	
	CPackManager *p = (CPackManager*)par;

	while(1)
	{
		while(p->m_connected != 2)
		{
			if(p->m_bThreadHeartBeatStop==TRUE) 
				return NULL;

			p->StartClient();
			p->StartSleep(p->m_hStopSleep,10*1000);//��10��
		}

		while(1)
		{
			if(p->m_bThreadHeartBeatStop == TRUE) 
				return NULL;

			int ret = p->DoKeeplivePack();
		
			if(ret == ERR_CONNECTIONLOST)
			{
				p->m_connected = 0;
				break;
			}

			p->StartSleep(p->m_hStopSleep,10*1000);//��10��
		}
	}
	
	
	return 0;
};	

/***********************************************
�����������߳�
***********************************************/
void CPackManager::StartHeartBeat(void)
{	
	if(m_bThreadHeartBeatRunning==TRUE)
	{
		CLog::Log(LOG_LEVEL_WARNING,"�߳������У�����Ҫ�ٴ���\n");
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
		CLog::Log(LOG_LEVEL_ERROR,"�����߳�ʧ�ܣ��������: %d\n", returnValue);
		m_bThreadHeartBeatStop = TRUE;
		m_bThreadHeartBeatRunning = FALSE;
		this->StopSleep(m_hStopSleep);
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"�����̴߳����ɹ�\n");
		m_bThreadHeartBeatStop = FALSE;
		m_bThreadHeartBeatRunning = TRUE;
	}
}

/***********************************************
�����������߳�
***********************************************/
void CPackManager::StopHeartBeat(void)
{
	if(m_bThreadHeartBeatRunning==FALSE)
	{
		CLog::Log(LOG_LEVEL_NOMAL,"�߳�û��������\n");
		return;
	}

	m_bThreadHeartBeatStop = TRUE;
	this->StopSleep(m_hStopSleep);
	int returnValue = pthread_join(m_ThreadHeartBeat, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"�߳��˳�ʧ�ܣ��������: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"�̳߳ɹ��˳�\n");
	}
	m_bThreadHeartBeatRunning = FALSE;
}


/***********************************************
���ж�sleep
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
	if(WaitForSingleObject(hHandle, dwMilliseconds)==WAIT_OBJECT_0)
	{
		ResetEvent(hHandle);
	}
}

void CPackManager::StopSleep(HANDLE hHandle)
{
	SetEvent(hHandle);
	CloseHandle(hHandle);
}

void CPackManager::InitLockSocket(void)
{
	InitializeCriticalSection(&m_csSocket);
}

void CPackManager::DelLockSocket(void)
{
	DeleteCriticalSection(&m_csSocket);
}

void CPackManager::LockSocket(void)
{
	EnterCriticalSection(&m_csSocket);
}

void CPackManager::UnLockSocket(void)
{
	LeaveCriticalSection(&m_csSocket);
}