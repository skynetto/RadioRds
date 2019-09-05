#ifndef Pty_h__
#define Pty_h__

#include <string>
#include <map>

#define PTYTYPESIZE 4
#define PTY_VALIDATE_LIMIT 2

class CPty
{
public:
	CPty(void);
	virtual ~CPty(void);
	void									Validate(const BYTE currentPty);
	void									Reset();
	void									UpdatePtyType(const BYTE diSegment, const BYTE diControlBit);
	const BYTE *							GetPtyType(void);
	const std::wstring &					GetPtyDisplayString(void) const;
private:

	const std::wstring						GetPtyDisplayString(const BYTE & pty) const;
	void									InitializePtyDisplayString(void);		

	std::map<unsigned short,std::wstring>	m_ptyDisplay;
	std::wstring							m_ptyDisplayString;
	BYTE									m_ptyType[PTYTYPESIZE];
	int										m_ptyValidateCounter;
	BYTE									m_ptyLast;
};
#endif // Pty_h__

