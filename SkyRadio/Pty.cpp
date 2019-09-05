#include "StdAfx.h"
#include "Pty.h"


CPty::CPty(void)
{
	Reset();
	InitializePtyDisplayString();
}


CPty::~CPty(void)
{
}

void CPty::InitializePtyDisplayString()
{
	m_ptyDisplay[1] = L"News";
	m_ptyDisplay[2] = L"Current Affairs";
	m_ptyDisplay[3] = L"Information";
	m_ptyDisplay[4] = L"Sport";
	m_ptyDisplay[5] = L"Education";
	m_ptyDisplay[6] = L"Drama";
	m_ptyDisplay[7] = L"Culture";
	m_ptyDisplay[8] = L"Science";
	m_ptyDisplay[9] = L"Varied";
	m_ptyDisplay[10] = L"Pop Music";
	m_ptyDisplay[11] = L"Rock Music";
	m_ptyDisplay[12] = L"Easy Listening";
	m_ptyDisplay[13] = L"Light Classic Music";
	m_ptyDisplay[14] = L"Serious Classic Music";
	m_ptyDisplay[15] = L"Other Music";
	m_ptyDisplay[16] = L"Weather & Metr";
	m_ptyDisplay[17] = L"Finance";
	m_ptyDisplay[18] = L"Children's Progs";
	m_ptyDisplay[19] = L"Social Affairs";
	m_ptyDisplay[20] = L"Religion";
	m_ptyDisplay[21] = L"Phone In";
	m_ptyDisplay[22] = L"Travel & Touring";
	m_ptyDisplay[23] = L"Leisure & Hobby";
	m_ptyDisplay[24] = L"Jazz Music";
	m_ptyDisplay[25] = L"Country Music";
	m_ptyDisplay[26] = L"National Music";
	m_ptyDisplay[27] = L"Oldies Music";
	m_ptyDisplay[28] = L"Folk Music";
	m_ptyDisplay[29] = L"Documentary";
	m_ptyDisplay[30] = L"Alarm Test";
	m_ptyDisplay[31] = L"Alarm - Alarm !";
}

const std::wstring CPty::GetPtyDisplayString( const BYTE & pty ) const
{
	auto it = m_ptyDisplay.find(pty);
	if(it != m_ptyDisplay.cend())
		return it->second;
	return std::wstring();
}

const std::wstring & CPty::GetPtyDisplayString() const
{
	return m_ptyDisplayString;
}


void CPty::Validate( const BYTE currentPty )
{
	if(currentPty != m_ptyLast)
	{
		m_ptyLast = currentPty;
		m_ptyValidateCounter = 1;
	}
	else
	{
		m_ptyValidateCounter++;
	}
	if(m_ptyValidateCounter > PTY_VALIDATE_LIMIT)
	{
		m_ptyValidateCounter = PTY_VALIDATE_LIMIT + 1;
		m_ptyDisplayString = GetPtyDisplayString(m_ptyLast);
	}
}

void CPty::Reset()
{
	memset(&m_ptyType,0,sizeof(BYTE) * PTYTYPESIZE);
	m_ptyLast = 0;
	m_ptyValidateCounter = 1;
	m_ptyDisplayString.clear();
}

void CPty::UpdatePtyType( const BYTE diSegment, const BYTE diControlBit )
{
	m_ptyType[diSegment] = diControlBit;
}

const BYTE * CPty::GetPtyType()
{
	return m_ptyType;
}
