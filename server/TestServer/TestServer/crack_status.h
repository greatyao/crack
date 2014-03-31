
#ifndef __CRACK_STATUS_H_
#define __CRACK_STATUS_H_


typedef enum CRACK_TASK_STATUS {

	CT_STATUS_READY = 1,   //任务的就绪状态
	CT_STATUS_FINISHED,		//任务完成状态	
	CT_STATUS_FAILURE,		//任务解密失败状态
	CT_STATUS_RUNNING,		//任务正在解密状态

//	CT_STATUS_STOPPED,		//任务停止状态  
	CT_STATUS_PAUSED,		//任务暂停解密状态
	CT_STATUS_DELETED,		//将任务设置为删除状态
	CT_STATUS_MAX
	
};

typedef enum CRACK_TASK_PRIORITY{


	CT_PRIORITY_NORMAL = 1,
	CT_PRIORITY_IMPORTANT,

};

typedef enum CRACK_BLOCK_STATUS{

	WI_STATUS_WAITING = 1,	//分配好工作项后，工作项的初始状态 
	WI_STATUS_READY,		//任务启动后，工作项的状态 ,start task
	WI_STATUS_RUNNING,		//工作项已经被计算节点请求，正在被计算节点处理中 , 被计算节点占用
	WI_STATUS_FINISHED,		//完成结束,包含有解密结果
	WI_STATUS_NO_PWD,		//解密结束但是未找到密码
	WI_STATUS_FAILURE,		//工作项执行失败

	WI_STATUS_NOT_NEED,		//任务不需要处理

//	WI_STATUS_STOPPED,		//任务被停止后，相关的工作项应该被停止
//	WI_STATUS_PAUSED,		//任务被暂停后，相关的工作项应该被暂停
//	WI_STATUS_DELETED,		//将工作项设置为删除状态
	
	WI_STATUS_MAX

};



//客户端类型
typedef enum CLIENT_TYPE {
	
	COMPUTE_TYPE_CLIENT,
	CONTROL_TYPE_CLIENT,
	SUPER_CONTROL_TYPE_CLIENT,

};

#endif



