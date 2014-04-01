#include "CrackBlock.h"
#include "crack_status.h"

CCrackBlock::CCrackBlock(void)
{
}

CCrackBlock::~CCrackBlock(void)
{
}


void CCrackBlock::Init(struct crack_block *pCrackBlock){

	
	algo = pCrackBlock->algo;
	charset = pCrackBlock->charset;
	type = pCrackBlock->type;
	special = pCrackBlock->special;


	memset(guid,0,40);
	memset(john,0,sizeof(struct crack_hash));
	
	memcpy(guid,pCrackBlock->guid,40);
	memcpy(john,pCrackBlock->john,sizeof(struct crack_hash));

	start = pCrackBlock->start;
	end = pCrackBlock->end;

	start2 = pCrackBlock->start2;
	end2 = pCrackBlock->end2;
		
	task = NULL;

	m_status = WI_STATUS_WAITING;

	m_progress = 0.0;
	m_speed = 0.0;
	m_remaintime = -1;

	memset(m_comp_guid,0,40);

}
