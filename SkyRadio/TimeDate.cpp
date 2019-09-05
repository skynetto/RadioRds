#include "StdAfx.h"
#include "TimeDate.h"
#ifdef _DEBUG
#include <sstream>
#endif

CTimeDate::CTimeDate(void)
{
	Reset();
}


CTimeDate::~CTimeDate(void)
{
}

void CTimeDate::AdjustTimeDate()
{
	const int daysInMonth = GetDaysInMonth(m_year,m_month);
	if(m_hour + m_localTimeOffset >= 24)
	{
		if(m_day + 1 > daysInMonth)
		{
			if(m_month + 1 > 12)
			{
				m_year++;
				m_month = 1;
			}
			else
				m_month++;
			m_day = 1;
		}
		else
			m_day++;	
		m_hour += m_localTimeOffset;
		m_hour %= 24;
	}
	else if(m_hour + m_localTimeOffset <= 0)
	{
		if(m_day - 1 == 0)
		{
			if(m_month - 1 == 0)
			{
				m_year--;
				m_month = 12;
			}
			else
				m_month--;
			m_day = GetDaysInMonth(m_year,m_month);
		}
		m_hour += 24 - m_localTimeOffset;
	}
	else
	{
		m_hour += m_localTimeOffset;
		m_hour %= 24;
	}
}


int CTimeDate::GetDaysInMonth( const int year, const int month )
{
	int daysInMonth = 0;
	switch(month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		{
			daysInMonth = 31;
		}			
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		{
			daysInMonth = 30;
		}
		break;
	case 2:
		{
			if((year % 400) == 0)
				daysInMonth = 29;
			else if(((year % 100) != 0) && ((year % 4) == 0))
				daysInMonth = 29;
			else
				daysInMonth = 28;
		}
		break;
	}
	return daysInMonth;
}

void CTimeDate::Reset()
{
	m_hour = 0;
	m_minute = 0;
	m_year = 0;
	m_month = 0;
	m_day = 0;
	m_localTimeOffset = 0;
	m_modifiedJulianDate = 0;
}

void CTimeDate::Update( const WORD _registerB, const WORD _registerC,const WORD _registerD )
{
	m_hour = ((_registerC & 0x01) << 4) | ((_registerD>>12) & 0x0F);
	m_minute = ((_registerD >> 6 ) & 0x3F);
	m_localTimeOffset = (_registerD & 0x1F) / 2;
	if((_registerD >> 5) & 0x01) 
		m_localTimeOffset *= -1;
	m_modifiedJulianDate = ((_registerB & 0x03) << 15) | ((_registerC >> 1) & 0x7FFF);
	m_year = static_cast<int>((static_cast<double>(m_modifiedJulianDate) - 15078.2) / 365.25);
	m_month = static_cast<int>(((static_cast<double>(m_modifiedJulianDate) - 14956.1) - (static_cast<double>(m_year) * 365.25)) / 30.6001);
	m_day = m_modifiedJulianDate - 14956 - static_cast<int>(static_cast<double>(m_year) * 365.25) - static_cast<int>(static_cast<double>(m_month) * 30.6001);
	int K = ((m_month == 14) || (m_month == 15)) ? 1 : 0;
	m_year += K;
	m_month -= 1 + (K*12);
	m_year += 1900;

	AdjustTimeDate();

#ifdef _DEBUG
	std::wstringstream str;
	str << L"Year: " << m_year << L" - "
		<< L"Month: " << m_month << L" - "
		<< L"Day: " << m_day << L" - "
		<< L"Hour: " << m_hour << L" - "
		<< L"Minutes: " << m_minute << std::endl;
	OutputDebugString(str.str().c_str());
#endif
}

int CTimeDate::GetMinute() const
{
	return m_minute;
}

int CTimeDate::GetHour() const
{
	return m_hour;
}

int CTimeDate::GetDay() const
{
	return m_day;
}

int CTimeDate::GetMonth() const
{
	return m_month;
}

int CTimeDate::GetYear() const
{
	return m_year;
}
