
#include "BufferTool.h"


/*

TASK_ALG_TYPE,		//�㷨����
TASK_HASH_INFO,		//���ܵĹ�����Ϣ
CONTROL_GUID,		//���ƽڵ�GUID
TASK_GUID,			//����ڵ�GUID
TASK_PRIORITY,		//�������ȼ�
TASK_MIN_LEN,		//��С����
TASK_MAX_LEN,		//��󳤶�
TASK_CHARSET,		//�ַ���
TASK_OP_RESULT,		//����������ɹ���ʧ��
TASK_FAIL_REASON,	//����ʧ��ԭ��

TASK_RUN_RESULT,	//�������н��
TASK_RUN_PROCESS,	//�������н���

TASK_COMP_CLIENT,	//����ڵ�

COMP_DEC_RESULT,	//���ܽ��
COMP_GUID,			//����ڵ�GUID
COMP_WI_GUID,		//workitem��GUID

*/

void getValue(int itemType,void *pDst,unsigned char *pSrc){

	switch(itemType��{


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


	switch(itemType��{


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


	switch(itemType��{


		case :

			break;
		case :


			break;
		default:


			return;


	}



	return ;
}
