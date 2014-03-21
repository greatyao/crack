
#ifndef _CLIENT_REQUEST_H_
#define _CLIENT_REQUEST_H_

#include <string>

//客户端请求数据结构

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



#endif 
