#pragma once


#define TOTAL_HASH_LEN 260

typedef enum Hash_STATUS_TYPE 
{	
	
	HASH_STATUS_READY,			//任务就绪
	HASH_STATUS_FINISH,			//任务完成
	HASH_STATUS_RUNNING,		//任务运行中
	
	HASH_STATUS_NO_PASS,		//任务结束，未找到密码
	HASH_STATUS_FAILURE,		//解密过程中发现错误

};


class CCrackHash
{
public:
	CCrackHash(void);
	~CCrackHash(void);
	
	void Init(unsigned char *john);

public:

	unsigned char m_john[TOTAL_HASH_LEN];
	unsigned char m_result[32];
	unsigned char m_status;

	float m_progress;

};
