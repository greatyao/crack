#include "BlockNotice.h"
#include <memory.h>

CBlockNotice::CBlockNotice(void)
{
	memset(m_guid, 0, sizeof(m_guid));
	m_status = 0;
}

CBlockNotice::~CBlockNotice(void)
{
}
