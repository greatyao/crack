#pragma once


#define TOTAL_HASH_LEN 260

typedef enum Hash_STATUS_TYPE 
{	
	
	HASH_STATUS_READY,			//�������
	HASH_STATUS_FINISH,			//�������
	HASH_STATUS_RUNNING,		//����������
	
	HASH_STATUS_NO_PASS,		//���������δ�ҵ�����
	HASH_STATUS_FAILURE,		//���ܹ����з��ִ���

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
