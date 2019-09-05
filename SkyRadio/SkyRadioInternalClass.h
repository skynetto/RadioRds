#pragma once

#include "RadioDevice.h"
#include "RadioSound.h"


class CSkyRadio 
{
public:
	CSkyRadio(HWND hWnd, const unsigned int userMessage);
	virtual ~CSkyRadio();
private:
	CRadioDevice		m_device;
	CRadioDevice		m_rdsDevice;
	CRadioSound			m_sound;

	bool				m_rdsDeviceFound;
public:
	bool	TurnOn(void);
	bool	TurnOff();
	bool	Seek(const bool & up);
	bool	Tune(const bool & up);
	bool	TuneFrequency(const unsigned int & frequency);
	bool	GetSingalStrength(unsigned int & rssi);
	bool	SetMute(const bool & mute);
	bool	SetPreferredSeekSettings(const BYTE seekThreshold, const BYTE sksnr, const BYTE skcnt);
	void	GetPreferredSeekSettings(BYTE & seekThreshold, BYTE & sksnr, BYTE & skcnt);
	bool	RaiseVolume(void);
	bool	LowerVolume(void);
	bool	SetVolume(const unsigned int & volume);
	bool	GetVolume(unsigned int & volume);
	const unsigned int GetCurrentStation(void) const;
	const unsigned int GetCurrentStationPI(void) const;
	const std::wstring & GetPtyDisplayString(void);
	const std::wstring & GetPtyNDisplayString(void);
	const std::wstring & GetPSDisplayString(void);
	const std::wstring & GetRegionString(void);
	const std::wstring & GetCountryString(void);
	const std::wstring & GetLanguageString(void);
	const std::wstring & GetRadioTextString(void);
	const SKYRADIO_TMCStruct & GetTMCData(void) const;
	const SKYRADIO_RTContent * GetRTPlusContent() const;
	const std::map<unsigned int, CEon> & GetEONList(void);
	const BYTE * GetPtyType(void);
	const std::set<unsigned int> & GetAFList(void);
	const CTimeDate & GetCurrentRadioTime(void) const;
	const bool GetTAFlag();
	const bool GetTPFlag();
	const bool GetMSFlag();
	const bool GetStereoFlag();
};