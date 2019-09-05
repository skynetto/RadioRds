#ifndef StringContructor_h__
#define StringContructor_h__

#include <string>
#include <vector>
#include <algorithm>

class CStringContructor
{
public:

	CStringContructor(size_t n, size_t limit);
	virtual ~CStringContructor(void);
	
	void									Update( const BYTE addr, const BYTE byte );
	void									Reset(void);
	const std::wstring &					GetDisplayString() const;

private:
	std::vector<BYTE>						m_Display;    // Displayed Program Service text
	std::vector<BYTE>						m_Tmp0;       // Temporary PS text (high probability)
	std::vector<BYTE>						m_Tmp1;       // Temporary PS text (low probability)
	std::vector<BYTE>						m_Cnt;        // Hit count of high probability PS text
	std::wstring							m_Text;
	size_t									N;
	size_t									LIMIT;
	std::wstring							str_to_wstr(const char *str, int length, UINT cp = CP_ACP);
};

#endif // StringContructor_h__

