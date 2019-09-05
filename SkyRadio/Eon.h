#ifndef Eon_h__
#define Eon_h__

#include "StringContructor.h"
#include "Pty.h"
#include "Af.h"
#include "Enums.h"

class CEon
{
public:
	CEon(void);
	virtual ~CEon(void);
	
	CAf											m_af;
	BYTE										m_tp;
	BYTE										m_ta;
	CPty										m_pty;
	CStringContructor							m_ps;
	int											m_validateCount;
};

#endif // Eon_h__
