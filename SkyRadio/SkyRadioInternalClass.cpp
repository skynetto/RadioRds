#include "StdAfx.h"
#include "SkyRadioInternalClass.h"


// This is the constructor of a class that has been exported.
// see SkyRadio.h for the class definition

CSkyRadio::CSkyRadio(HWND hWnd, const unsigned int userMessage)
	: m_device(hWnd,userMessage,"settingsp")
	, m_rdsDevice(hWnd,userMessage,"settingss")
	, m_rdsDeviceFound(false)
{
}


CSkyRadio::~CSkyRadio()
{

}

bool CSkyRadio::TurnOn(void)
{
	if(m_device.InitializeRadioDevice(std::wstring()))
	{
		m_rdsDeviceFound = m_rdsDevice.InitializeRadioDevice(m_device.GetDevicePath());
		return m_sound.InitializeSound(m_device.GetDeviceName());
	}
	return false;
}

bool CSkyRadio::Seek( const bool & up )
{
	return m_device.Seek(up);
}

bool CSkyRadio::TurnOff()
{
	return m_device.Mute(true);
}

const unsigned int CSkyRadio::GetCurrentStation() const
{
	return m_device.GetCurrentStation();
}

bool CSkyRadio::Tune( const bool & up )
{
	return m_device.Tune(up);
}

bool CSkyRadio::GetSingalStrength( unsigned int & rssi )
{
	if((rssi = m_device.GetSignalStrength()) != 0)
		return true;
	return false;
}

bool CSkyRadio::SetMute( const bool & mute )
{
	return m_device.Mute(mute);
}

const std::wstring & CSkyRadio::GetPtyDisplayString()
{
	return m_device.GetPtyDisplayString();
}

const bool CSkyRadio::GetTAFlag()
{
	return m_device.GetTAFlag();
}

const bool CSkyRadio::GetTPFlag()
{
	return m_device.GetTPFlag();
}

const std::wstring & CSkyRadio::GetPSDisplayString()
{
	return m_device.GetPSDisplayString();
}

const bool CSkyRadio::GetMSFlag()
{
	return m_device.GetMSFlag();
}

const std::set<unsigned int> & CSkyRadio::GetAFList()
{
	return m_device.GetAFList();
}

const std::wstring & CSkyRadio::GetPtyNDisplayString()
{
	return m_device.GetPtyNDisplayString();
}

const std::wstring & CSkyRadio::GetRegionString()
{
	return m_device.GetRegionString();
}

const std::wstring & CSkyRadio::GetCountryString()
{
	return m_device.GetCountryString();
}

const unsigned int CSkyRadio::GetCurrentStationPI() const
{
	return m_device.GetCurrentStationPI();
}

bool CSkyRadio::TuneFrequency( const unsigned int & frequency )
{
	return m_device.SetFrequency(frequency);
}

const std::wstring & CSkyRadio::GetLanguageString()
{
	return m_device.GetLanguageString();
}

const CTimeDate & CSkyRadio::GetCurrentRadioTime() const
{
	return m_device.GetCurrentRadioTime();
}

const BYTE * CSkyRadio::GetPtyType()
{
	return m_device.GetPtyType();
}

const std::wstring & CSkyRadio::GetRadioTextString()
{
	return m_device.GetRadioTextString();
}

bool CSkyRadio::SetPreferredSeekSettings( const BYTE seekThreshold, const BYTE sksnr, const BYTE skcnt )
{
	return m_device.SetPreferredSeekSettings(seekThreshold,sksnr,skcnt);
}

void CSkyRadio::GetPreferredSeekSettings( BYTE & seekThreshold, BYTE & sksnr, BYTE & skcnt )
{
	return m_device.GetPreferredSeekSettings(seekThreshold,sksnr,skcnt);
}

bool CSkyRadio::RaiseVolume()
{
	return m_sound.RaiseVolume();
}

bool CSkyRadio::LowerVolume()
{
	return m_sound.LowerVolume();
}

bool CSkyRadio::SetVolume( const unsigned int & volume )
{
	return m_sound.SetVolume(volume);
}

bool CSkyRadio::GetVolume( unsigned int & volume )
{
	return m_sound.GetVolume(volume);
}

const std::map<unsigned int, CEon> & CSkyRadio::GetEONList()
{
	return m_device.GetEONList();
}

const SKYRADIO_TMCStruct & CSkyRadio::GetTMCData() const
{
	return m_device.GetTMCData();
}

const bool CSkyRadio::GetStereoFlag()
{
	return m_device.GetStereoFlag();
}

const SKYRADIO_RTContent * CSkyRadio::GetRTPlusContent() const
{
	return m_device.GetRTPlusContent();
}
