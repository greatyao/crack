#include "CrackBlock.h"
#include "macros.h"
#include "macros.h"

CCrackBlock::CCrackBlock(void)
{
}

CCrackBlock::~CCrackBlock(void)
{
}


void CCrackBlock::Init(struct crack_block *pCrackBlock){

	//ֱ�Ӱ��սṹ�帳ֵ����һ���ڴ漴�ɣ�����һ�������θ�ֵ
	crack_block* parent = this;
	memcpy(parent, pCrackBlock, sizeof(crack_block));
	parent->task = NULL;

	m_status = WI_STATUS_WAITING;

	m_progress = 0.0;
	m_speed = 0.0;
	//m_remaintime = -1;
	m_remaintime = 0;

	memset(m_comp_guid,0,40);

}
