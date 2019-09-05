#include "StdAfx.h"
#include "StringContructor.h"


CStringContructor::CStringContructor(size_t n, size_t limit)
	: N(n)
	, LIMIT(limit)
{
	Reset();
}

CStringContructor::~CStringContructor()
{

}

void CStringContructor::Update( const BYTE addr, const BYTE byte )
{
	BYTE i = 0;
	BYTE textChange = 0; // indicates if the text is in transition
	BYTE complete = 1; // indicates that the text is ready to be displayed

	if(m_Tmp0[addr] == byte)
	{
		// The new byte matches the high probability byte
		if(m_Cnt[addr] < LIMIT)
		{
			m_Cnt[addr]++;
		}
		else
		{
			// we have received this byte enough to max out our counter
			// and push it into the low probability array as well
			m_Cnt[addr] = LIMIT;
			m_Tmp1[addr] = byte;
		}
	}
	else if(m_Tmp1[addr] == byte)
	{
		// The new byte is a match with the low probability byte. Swap
		// them, reset the counter and flag the text as in transition.
		// Note that the counter for this character goes higher than
		// the validation limit because it will get knocked down later
		if(m_Cnt[addr] >= LIMIT)
		{
			textChange = 1;
		}
		m_Cnt[addr] = LIMIT + 1;
		m_Tmp1[addr] = m_Tmp0[addr];
		m_Tmp0[addr] = byte;
	}
	else if(!m_Cnt[addr])
	{
		// The new byte is replacing an empty byte in the high
		// probability array
		m_Tmp0[addr] = byte;
		m_Cnt[addr] = 1;
	}
	else
	{
		// The new byte doesn't match anything, put it in the
		// low probability array.
		m_Tmp1[addr] = byte;
	}

	if(textChange)
	{
		// When the text is changing, decrement the count for all
		// characters to prevent displaying part of a message
		// that is in transition.
		for(i = 0; i < N; i++)
		{
			if(m_Cnt[i] > 1)
			{
				m_Cnt[i]--;
			}
		}
	}

	// The text is incomplete if any character in the high
	// probability array has been seen fewer times than the
	// validation limit.
	for (i = 0; i < N; i++)
	{
		if(m_Cnt[i] < LIMIT)
		{
			complete = 0;
			break;
		}
	}

	// If the text in the high probability array is complete
	// copy it to the display array
	if (complete)
	{
		m_Display = m_Tmp0;
		m_Text = str_to_wstr(reinterpret_cast<char *>(&m_Display[0]),N);
	}
}

const std::wstring & CStringContructor::GetDisplayString() const
{
	return m_Text;
}

void CStringContructor::Reset()
{
	m_Text.clear();
	m_Display.assign(N,0);
	m_Tmp0.assign(N,0);
	m_Tmp1.assign(N,0);
	m_Cnt.assign(N,0);
}


std::wstring CStringContructor::str_to_wstr( const char *str, int length, UINT cp /*= CP_ACP*/ )
{
	int len = MultiByteToWideChar(cp, 0, str, length, 0, 0);
	if (!len)
		return L"";

	std::vector<wchar_t> wbuff(len + 1);
	// NOTE: this does not NULL terminate the string in wbuff, but this is ok
	//       since it was zero-initialized in the vector constructor
	if (!MultiByteToWideChar(cp, 0, str, length, &wbuff[0], len))
		return L"";

	return &wbuff[0];
}
