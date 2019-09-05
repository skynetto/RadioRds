#include "StdAfx.h"
#include "RadioText.h"
#include <algorithm>

CRadioText::CRadioText(void)
{
	Reset();
}


CRadioText::~CRadioText(void)
{
}

void CRadioText::Update( const BYTE abFlag, const BYTE count, const BYTE address, const BYTE errorFlags, const BYTE *byte )
{
	BYTE i;
	BYTE textChange = 0; // indicates if the Radio Text is changing

	if (abFlag != m_rtFlag && m_rtFlagValid)
	{
		// If the A/B message flag changes, try to force a display
		// by increasing the validation count of each byte
		// and stuffing a space in place of every NUL char
		for (i = 0; i < RADIOTEXTSIZE; i++)
		{
			if (!m_rtTmp0[i])
			{
				m_rtTmp0[i] = ' ';
				m_rtCnt[i]++;
			}
		}
		for (i = 0 ; i < RADIOTEXTSIZE; i++)
		{
			m_rtCnt[i]++;
		}
		DisplayRadioText();

		// Wipe out the cached text
		memset(m_rtTmp0,0,sizeof(BYTE) * RADIOTEXTSIZE);
		memset(m_rtTmp1,0,sizeof(BYTE) * RADIOTEXTSIZE);
		memset(m_rtCnt,0,sizeof(BYTE) * RADIOTEXTSIZE);
	}

	m_rtFlag = abFlag;    // Save the A/B flag
	m_rtFlagValid = 1;    // Our copy of the A/B flag is now valid

	for(i = 0; i < count; i++)
	{
		BYTE errCount(0);
		BYTE blerMax(0);
		// Choose the appropriate block. Count > 2 check is necessary for 2B groups
		if ((i < 2) && (count > 2))
		{
			errCount = ERRORS_CORRECTED(errorFlags,BLOCK_C);
			blerMax = CORRECTED_THREE_TO_FIVE;
		}
		else
		{
			errCount = ERRORS_CORRECTED(errorFlags,BLOCK_D);
			blerMax = CORRECTED_THREE_TO_FIVE;
		}

		if (errCount <= blerMax)
		{
			// The new byte matches the high probability byte
			if(m_rtTmp0[address + i] == byte[i])
			{
				if(m_rtCnt[address + i] < RT_VALIDATE_LIMIT)
				{
					m_rtCnt[address + i]++;
				}
				else
				{
					// we have received this byte enough to max out our counter
					// and push it into the low probability array as well
					m_rtCnt[address + i] = RT_VALIDATE_LIMIT;
					m_rtTmp1[address + i] = byte[i];
				}
			}
			else if(m_rtTmp1[address + i] == byte[i])
			{
				// The new byte is a match with the low probability byte. Swap
				// them, reset the counter and flag the text as in transition.
				// Note that the counter for this character goes higher than
				// the validation limit because it will get knocked down later
				if(m_rtCnt[address + i] >= RT_VALIDATE_LIMIT)
				{
					textChange = 1;
					m_rtCnt[address + i] = RT_VALIDATE_LIMIT + 1;
				}
				else
				{
					m_rtCnt[address + i] = RT_VALIDATE_LIMIT;
				}
				m_rtTmp1[address + i] = m_rtTmp0[address + i];
				m_rtTmp0[address + i] = byte[i];
			}
			else if(!m_rtCnt[address + i])
			{
				// The new byte is replacing an empty byte in the high
				// probability array
				m_rtTmp0[address + i] = byte[i];
				m_rtCnt[address + i] = 1;
			}
			else
			{
				// The new byte doesn't match anything, put it in the
				// low probability array.
				m_rtTmp1[address + i] = byte[i];
			}
		}
	}

	if(textChange)
	{
		// When the text is changing, decrement the count for all
		// characters to prevent displaying part of a message
		// that is in transition.
		for(i = 0; i < RADIOTEXTSIZE; i++)
		{
			if(m_rtCnt[i] > 1)
			{
				m_rtCnt[i]--;
			}
		}
	}

	// Display the Radio Text
	DisplayRadioText();

}

void CRadioText::DisplayRadioText()
{
	BYTE rtComplete = 1;
	BYTE i;

	// The Radio Text is incomplete if any character in the high
	// probability array has been seen fewer times than the
	// validation limit.
	for (i=0; i < RADIOTEXTSIZE; i++)
	{
		if(m_rtCnt[i] < RT_VALIDATE_LIMIT)
		{
			rtComplete = 0;
			break;
		}
		if(m_rtTmp0[i] == 0x0d)
		{
			// The array is shorter than the maximum allowed
			break;
		}
	}

	// If the Radio Text in the high probability array is complete
	// copy it to the display array
	if (rtComplete)
	{
		for (i=0; i < RADIOTEXTSIZE; i += 2)
		{
			if ((m_rtDisplay[i] != 0x0d) && (m_rtDisplay[i+1] != 0x0d))
			{
				m_rtDisplay[i] = m_rtTmp0[i+1];
				m_rtDisplay[i+1] = m_rtTmp0[i];
			}
			else
			{
				m_rtDisplay[i] = m_rtTmp0[i];
				m_rtDisplay[i+1] = m_rtTmp0[i+1];
			}

			if ((m_rtDisplay[i] == 0x0d) || (m_rtDisplay[i+1] == 0x0d))
				i = RADIOTEXTSIZE;
		}

		// Wipe out everything after the end-of-message marker
		for (i++; i < RADIOTEXTSIZE; i++)
		{
			m_rtDisplay[i] = 0;
			m_rtCnt[i]     = 0;
			m_rtTmp0[i]    = 0;
			m_rtTmp1[i]    = 0;
		}

		std::replace_if(m_rtDisplay,
						m_rtDisplay + RADIOTEXTSIZE,
						[&](size_t i)
						{
							return m_rtDisplay[i] == 0x00;
						},
						0x20);		
		m_rtDisplayString = str_to_wstr(reinterpret_cast<char *>(m_rtDisplay),RADIOTEXTSIZE);
	}
}

void CRadioText::Reset()
{
	memset(m_rtDisplay,0,sizeof(BYTE) * RADIOTEXTSIZE);
	memset(m_rtTmp0,0,sizeof(BYTE) * RADIOTEXTSIZE);
	memset(m_rtTmp1,0,sizeof(BYTE) * RADIOTEXTSIZE);
	memset(m_rtCnt,0,sizeof(BYTE) * RADIOTEXTSIZE);
	m_rtDisplayString.clear();
	m_rtFlag = 0;
	m_rtFlagValid = 0;
}

const std::wstring & CRadioText::GetRadioTextString() const
{
	return m_rtDisplayString;
}
