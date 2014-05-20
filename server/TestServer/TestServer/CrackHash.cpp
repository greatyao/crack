#include "CrackHash.h"
#include "macros.h"
#include <string.h>

CCrackHash::CCrackHash(void)
{
	memset(m_john, 0, sizeof(m_john));
	memset(m_result, 0, sizeof(m_result));
	m_status = HASH_STATUS_READY;
}

CCrackHash::~CCrackHash(void)
{
}

void CCrackHash::Init(unsigned char *john){

	memset(m_john,0,TOTAL_HASH_LEN);
	memcpy(m_john,john,196);
	memset(this->m_result,0,sizeof(m_result));
	m_status = HASH_STATUS_READY;
	m_progress = 0.0;
}