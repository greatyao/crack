#pragma once

#include "UnifBuffer.h"

class CCmdBuf
{

	LPBYTE m_pCmdData;
	UINT m_nCmdLen;

public:
	
	CCmdBuf(void);
	CCmdBuf(BYTE cmd,CUnifBuffer unifbuffer);
	~CCmdBuf(void);

	UINT GetCmdLen(void);
	LPBYTE GetCmdData(void);
	

};
