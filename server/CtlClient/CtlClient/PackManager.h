#pragma once
#include "ReqPacket.h"
#include "macros.h"
#include "algorithm_types.h"
#include "ResPacket.h"
#include "SocketClient.h"


class CPackManager
{
public:
	CPackManager(void);
	~CPackManager(void);

	int DoLoginPack(client_login_req req);

	int DoKeeplivePack();

	int DoTaskUploadPack(crack_task req,task_upload_res *res);

	int GenTaskStartPack(task_start_req req,task_status_res *res);

	int GenTaskStopPack(task_stop_req req,task_status_res *res);

	int GenTaskPausePack(task_pause_req req,task_status_res *res);

	int GenTaskDeletePackt(task_delete_req req,task_status_res *res);

	int GenTaskResultPack(task_result_req req,task_status_res *res);

	int GenTaskStatusPack(task_status_info **res);

	int GenClientStatusPack(compute_node_info **res);

	int GenFileUploadPack(file_upload_req req,file_upload_res *res);

	int GenFileUploadStart(file_upload_end_res *res);

public:

	char m_cur_upload_guid[40];
	void * m_cur_server_file;
	unsigned int m_cur_upload_file_len;
	char m_cur_local_file[256];
	CSocketClient m_sockclient;

};

extern CPackManager g_packmanager;
