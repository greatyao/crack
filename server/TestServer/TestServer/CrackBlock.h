#pragma once
#include "algorithm_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class CCrackBlock : public crack_block
{
public:
	CCrackBlock(void);
	~CCrackBlock(void);
	
	void Init(struct crack_block *pCrackBlock);


public:
	
	//×´Ì¬ÐÅÏ¢
	unsigned char m_status;
	
	float	m_progress;

	float	m_speed;

	unsigned int m_remaintime;

	
	char m_comp_guid[40];

	unsigned int m_starttime;
	unsigned int m_finishtime;
};
