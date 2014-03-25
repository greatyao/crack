#pragma once

class CPacketManager
{
public:
	
//////解析接收数据的相关接口

	//控制节点上传任务请求结构
	virtual CtlTaskUpload * GetCtlTaskUploadReq(BYTE *pdata,int len) = 0;

	//控制节点开始文件上传任务请求结构
	//控制节点上传文件请求结构
	//控制节点结束文件上传请求结构
	virtual CltUploadFileStart * GetCtlTaskFileUploadStart(BYTE *pdata,int len) = 0;
	virtual CltUploadFileBlockInfo * GetCtlTaskFileUploadTran(BYTE *pdata,int len) = 0;
	virtual CltUploadFileEnd * GetCtlTaskFileUploadEnd(BYTE *pdata,int len) = 0;

	//控制节点开始任务请求结构
	virtual CtlTaskStart * GetCtlTaskStartReq(BYTE *pdata,int len) = 0;

	//控制节停止任务请求结构
	virtual CtlTaskStop * GetCtlTaskStopReq(BYTE *pdata,int len) = 0;

	//控制节删除任务请求结构
	virtual CtlTaskDel * GetCtlTaskDelReq(BYTE *pdata,int len) = 0;

	//控制节点暂停任务请求结构
	virtual CtlTaskPause * GetCtlTaskPauseReq(BYTE *pdata,int len) = 0;
	
	//获取任务执行结果请求结构
	virtual CtlTaskGuid * GetCtlTaskResultReq(BYTE *pdata,int len) = 0;
	
	//获取正在解密任务状态请求结构
	//无参数

	//获取任务计算节点信息请求结构
	//无参数

	//处理计算节点请求
	//获取解密成功完成请求结构
	virtual CompWIRecovered * GetCompWIDecOKReq(BYTE *pdata,int len) = 0;

	//获取解密完成、未找到密码请求结构
	virtual CompWIUnRecovered * GetCompWIDecNoPwdReq(BYTE *pdata,int len) = 0;

	//获取解密失败请求结构
	virtual WorkItemInfo * GetCompWIReq(BYTE *pdata,int len) = 0;

	//计算节点下载文件请求
	virtual CltDownloadFileStart *GetCompDownloadStart(BYTE *pdata, int len) = 0;
	virtual CltDownloadFileBlockInfo *GetCompDownloadBlockInfo(BYTE *pdata, int len) = 0;
	virtual CltDownloadFileEnd *GetCompDownloadEnd(BYTE *pdata, int len) = 0;

	//获取新任务请求结构
	virtual CompWIFailed * GetCompWIFailedReq(BYTE *pdata,int len) = 0;

//////产生应答数据的相关接口

	//产生任务上传应答数据
//	virtual LPBYTE GetTaskUploadRes()=0;

	//产生任务开始应答数据
//	virtual LPBYTE GetTaskStartRes()=0;

	//产生任务停止应答数据
//	virtual LPBYTE GetTaskUploadRes()=0;

	//产生任务删除应答数据
//	virtual LPBYTE GetTaskStartRes()=0;

	//产生任务暂停应答数据
//	virtual LPBYTE GetTaskUploadRes()=0;

	//产生获取任务结果应答数据
//	virtual LPBYTE GetTaskStartRes()=0;

	//产生获取任务状态应答数据
//	virtual LPBYTE GetTaskUploadRes()=0;

	//产生获取客户端信息应答数据
//	virtual LPBYTE GetTaskStartRes()=0;


///////产生计算节点相关应答数据

	//产生解密成功应答数据
//	virtual LPBYTE GetTaskUploadRes()=0;

	//产生解密完成未找到密码应答数据
//	virtual LPBYTE GetTaskStartRes()=0;

	//产生请求新解密任务应答数据
//	virtual LPBYTE GetTaskUploadRes()=0;

	//产生解密失败应答数据
//	virtual LPBYTE GetTaskStartRes()=0;

	//获取任务计算节点信息
//	CompNodeInfo *ReqGetClientList();

	//处理计算节点请求

	//处理解密成功完成请求
//	WorkItemGuid * ReqDecRecovered(CompWIRecovered *pwirec);

	//处理解密完成，单位找到结果
//	WorkItemGuid * ReqDecUnRecovered(CompWIUnRecovered *pwiunrec);

	//请求工作项任务
//	WorkItemInfo * ReqWorkitems(CompGuid *pguid);
	

	//处理解密失败

	
/*	
//任务上传请求
typedef struct _CTL_CLIENT_TASK_UPLOAD_{

	std::string m_algorithm;
	std::string m_hashinfo;
	std::string m_ctlguid;


}CtlTaskUpload;

//任务开始请求，服务器响应该请求后，对任务进行划分
typedef struct _CTL_CLIENT_TASK_START_{

	std::string m_guid;
	unsigned char m_priority;
	std::string m_start;
	std::string m_end;
	std::string m_charset;


}CtlTaskStart;

//任务停止请求
typedef struct _CTL_CLIENT_TASK_STOP_{

	std::string m_guid;

}CtlTaskStop;

//任务删除请求
typedef struct _CTL_CLIENT_TASK_DEL_{

	std::string m_guid;

}CtlTaskDel;


//任务暂停请求
typedef struct _CTL_CLIENT_TASK_PAUSE_{

	std::string m_guid;

}CtlTaskPause;


//得到任务结果
typedef struct _CTL_CLIENT_TASK_GUID_{

	std::string m_guid;
}CtlTaskGuid;



//得到正在执行任务信息
//无


//得到客户端列表

//无

//计算节点相关请求
//得到工作项完成请求,解密结果产生

typedef struct _COMP_CLIENT_WORKITEM_RECOVERED_{

	unsigned char m_wi_status;	//工作项的状态
	std::string m_result;
	std::string m_guid;


}CompWIRecovered;

//得到工作项完成，但未解密出结果
typedef struct _COMP_CLIENT_WORKITEM_UNRECOVERED_{

	unsigned char m_wi_status;	
	std::string m_guid;

}CompWIUnRecovered;


//请求分发新工作项
typedef struct _COMP_CLIENT_GUID_{

	std::string m_guid;

}CompGuid;

//工作项解密失败通知
typedef struct _COMP_CLIENT_WORKITEM_FAILRED_{

	std::string m_wi_guid;
	std::string m_comp_guid;

}CompWIFailed;
*/

	
};
