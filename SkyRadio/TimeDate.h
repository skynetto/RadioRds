#ifndef TimeDate_h__
#define TimeDate_h__


class CTimeDate
{
public:
	CTimeDate(void);
	virtual ~CTimeDate(void);

	void						Reset(void);
	void						Update(const WORD _registerB, const WORD _registerC,const WORD _registerD);
	int							GetMinute() const;
	int							GetHour() const;
	int							GetDay() const;
	int							GetMonth() const;
	int							GetYear() const;

private:
	
	int							m_hour;
	int							m_minute;
	int							m_year;
	int							m_month;
	int							m_day;
	int							m_localTimeOffset;
	int							m_modifiedJulianDate;

	int							GetDaysInMonth( const int year, const int month );
	void						AdjustTimeDate();
};
#endif // TimeDate_h__

