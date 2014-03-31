#pragma once
#include "clientinfo.h"

class CCompClient :
	public CClientInfo
{
public:
	CCompClient(void);
	~CCompClient(void);

public:
	int m_gputhreads;
	int m_cputhreads;

};
