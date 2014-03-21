
#include "BufferTool.h"


/*

TASK_ALG_TYPE,		//算法类型
TASK_HASH_INFO,		//解密的哈西信息
CONTROL_GUID,		//控制节点GUID
TASK_GUID,			//任务节点GUID
TASK_PRIORITY,		//任务优先级
TASK_MIN_LEN,		//最小长度
TASK_MAX_LEN,		//最大长度
TASK_CHARSET,		//字符集
TASK_OP_RESULT,		//操作结果，成功和失败
TASK_FAIL_REASON,	//操作失败原因

TASK_RUN_RESULT,	//任务运行结果
TASK_RUN_PROCESS,	//任务运行进度

TASK_COMP_CLIENT,	//计算节点

COMP_DEC_RESULT,	//解密结果
COMP_GUID,			//计算节点GUID
COMP_WI_GUID,		//workitem的GUID

*/

void getValue(int itemType,void *pDst,unsigned char *pSrc){

	switch(itemType）{


		case CONTROL_GUID:
		case TASK_GUID:
		case COMP_GUID:
		case COMP_WI_GUID:
			memcpy(pDst,pSrc,GUID_LEN);
			break;
		case TASK_ALG_TYPE:
			memcpy(pDst,pSrc,ALG_TYPE_LEN);
			break;
		case TASK_HASH_INFO:
			memcpy(pDst,pSrc,HASH_INFO_LEN);
			break;
		case TASK_PRIORITY:
			memcpy(pDst,pSrc,TASK_PRI_LEN);
			break;
		case TASK_MIN_LEN:
		case TASK_MAX_LEN:
			memcpy(pDst,pSrc,LENGTH_LEN);
			break;
		case TASK_HASH_INFO:
			memcpy(pDst,pSrc,HASH_INFO_LEN);
			break;
		case TASK_PRIORITY:
			memcpy(pDst,pSrc,TASK_PRI_LEN);
			break;

		default:


			return;


	}
}

void getLValue(int itemType,int *itemLen,void *pDst,unsigned char *pSrc){




	return;
}

void setValue(int itemType,void *pItem,unsigned char *pDst){




	return;
}

void seLValue(int itemType,int itemLen,void *pItem,unsigned char *pDst){






	return;
}




void getContent(int itemType,void *pDst,unsigned char *pSrc){


	switch(itemType）{


		case CONTROL_GUID:
		case TASK_GUID:
		case COMP_GUID:
		case COMP_WI_GUID:
			memcpy(pDst,pSrc,);

			break;
		case TASK_ALG_TYPE:

			memcpy(pDst,pSrc,ALG_TYPE_LEN);
			break;
		default:


			return;


	}

	return;
}

void getContent(int itemType,void *pDst,unsigned char *pSrc,int itemLen){


	switch(itemType）{


		case :

			break;
		case :


			break;
		default:


			return;


	}



	return ;
}
