#ifndef RadioText_h__
#define RadioText_h__

#include <string>
#include "Enums.h"

#define RADIOTEXTSIZE 64
#define RT_VALIDATE_LIMIT 2

class CRadioText
{
public:
	CRadioText(void);
	virtual ~CRadioText(void);

	void									Update(const BYTE abFlag, const BYTE count, const BYTE address, const BYTE errorFlags, const BYTE *byte);
	void									Reset(void);
	const std::wstring &					GetRadioTextString() const;
private:

	BYTE									m_rtDisplay[RADIOTEXTSIZE];		// Displayed Radio Text
	BYTE									m_rtTmp0[RADIOTEXTSIZE];		// Temporary Radio Text (high probability)
	BYTE									m_rtTmp1[RADIOTEXTSIZE];		// Temporary radio text (low probability)
	BYTE									m_rtCnt[RADIOTEXTSIZE];			// Hit count of high probability radio text
	BYTE									m_rtFlag;						// Radio Text A/B flag
	BYTE									m_rtFlagValid;					// Radio Text A/B flag is valid
	std::wstring							m_rtDisplayString;

	void									DisplayRadioText(void);
};
#endif // RadioText_h__

