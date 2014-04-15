#pragma once


#define TOTAL_HASH_LEN 260


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
