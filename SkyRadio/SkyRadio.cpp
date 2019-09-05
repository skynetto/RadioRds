// SkyRadio.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SkyRadio.h"
#include "SkyRadioInternalClass.h"

static CSkyRadio *m_radio = NULL;

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_TurnOn(HWND hWnd, const unsigned int userMessage)
{
	if(m_radio) 
	{
		delete m_radio;
		m_radio = NULL;
	}
	m_radio = new CSkyRadio(hWnd, userMessage);
	return m_radio->TurnOn() ? SKYRADIO_OK : SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_TurnOff()
{
	if(m_radio)
	{
		m_radio->TurnOff();
		delete m_radio;
		m_radio = NULL;
		return SKYRADIO_OK;
	}
	
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_Seek( const bool seekUp )
{
	if(m_radio)
	{
		return m_radio->Seek(seekUp) ? SKYRADIO_OK : SKYRADIO_SEEK_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCurrentStation( unsigned int * currentFreq )
{
	if(m_radio)
	{
		if(currentFreq)
		{
			*currentFreq = m_radio->GetCurrentStation();
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}


SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_SetCurrentStation( const unsigned int currentFreq )
{
	if(m_radio)
	{
		return m_radio->TuneFrequency(currentFreq) ? SKYRADIO_OK : SKYRADIO_SEEK_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}


SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_Tune( const bool tuneUp )
{
	if(m_radio)
	{
		return m_radio->Tune(tuneUp) ? SKYRADIO_OK : SKYRADIO_SEEK_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetSignalStrength( unsigned int * rssi )
{
	if(m_radio)
	{
		if(rssi)
			return m_radio->GetSingalStrength(*rssi) ? SKYRADIO_OK : SKYRADIO_SIGNAL_ERROR;
		else
			return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCurrentStationPI( unsigned int * currentPI )
{
	if(m_radio)
	{
		if(currentPI)
		{
			*currentPI = m_radio->GetCurrentStationPI();
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_Mute( const bool mute )
{
	if(m_radio)
	{
		return m_radio->SetMute(mute) ? SKYRADIO_OK : SKYRADIO_NOT_INITIALIZED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPTYString( wchar_t * ptyString, size_t *size )
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetPtyDisplayString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(ptyString)
				{
					*size = str.length() + 1;
					wcsncpy_s(ptyString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}



SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPTYType( unsigned char * ptyType )
{
	if(m_radio)
	{
		if(ptyType)
		{
			const unsigned char *ptyTypePtr = m_radio->GetPtyType();
			memcpy(ptyType,ptyTypePtr,sizeof(unsigned char) * 4);
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}


SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetTAFlag( bool * taFlag )
{
	if(m_radio)
	{
		if(taFlag)
		{
			*taFlag = m_radio->GetTAFlag();
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetTPFlag( bool * tpFlag )
{
	if(m_radio)
	{
		if(tpFlag)
		{
			*tpFlag = m_radio->GetTPFlag();
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}


SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetStereoFlag( bool * stereoFlag )
{
	if(m_radio)
	{
		if(stereoFlag)
		{
			*stereoFlag = m_radio->GetStereoFlag();
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPSString(wchar_t * psString, size_t *size)
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetPSDisplayString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(psString != NULL)
				{
					*size = str.length() + 1;
					wcsncpy_s(psString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetMSFlag( bool * msFlag )
{
	if(m_radio)
	{
		if(msFlag)
		{
			*msFlag = m_radio->GetMSFlag();
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetAFList(unsigned int *afList, size_t *size)
{
	if(m_radio)
	{
		if(size)
		{
			const std::set<unsigned int> & set_afList = m_radio->GetAFList();
			if(*size < set_afList.size())
			{
				*size = set_afList.size();
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(afList)
				{
					*size = set_afList.size();
					std::copy(set_afList.begin(),set_afList.end(),afList);
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPTYNString(wchar_t * ptynString, size_t *size)
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetPtyNDisplayString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(ptynString)
				{
					*size = str.length() + 1;
					wcsncpy_s(ptynString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRegionString( wchar_t * regionString, size_t *size )
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetRegionString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(regionString)
				{
					*size = str.length() + 1;
					wcsncpy_s(regionString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCountryString( wchar_t * countryString, size_t *size )
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetCountryString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(countryString)
				{
					*size = str.length() + 1;
					wcsncpy_s(countryString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetLanguageString( wchar_t * languageString, size_t *size )
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetLanguageString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(languageString)
				{
					*size = str.length() + 1;
					wcsncpy_s(languageString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCurrentRadioTime( int *year, int *month, int *day, int *hour, int *minute )
{
	if(m_radio)
	{
		if(!year || !month || !day || !hour || !minute)
			return SKYRADIO_NULL_POINTER_PROVIDED;
		else
		{
			const CTimeDate & tds = m_radio->GetCurrentRadioTime();
			*year = tds.GetYear();
			*month = tds.GetMonth();
			*day = tds.GetDay();
			*hour = tds.GetHour();
			*minute = tds.GetMinute();
			return SKYRADIO_OK;
		}
	}
	return SKYRADIO_NOT_INITIALIZED;
}


SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRadioTextString( wchar_t * radioTextString, size_t *size )
{
	if(m_radio)
	{
		if(size)
		{
			const std::wstring & str = m_radio->GetRadioTextString();
			if(*size <= str.length())
			{
				*size = str.length() + 1;
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
			else
			{
				if(radioTextString)
				{
					*size = str.length() + 1;
					wcsncpy_s(radioTextString,*size,str.c_str(),str.length());
					return SKYRADIO_OK;
				}
				return SKYRADIO_NULL_POINTER_PROVIDED;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}


SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_SetRadioSettings( SKYRADIO_SETTINGS *settings )
{
	if(m_radio)
	{
		if(settings)
		{
			return m_radio->SetPreferredSeekSettings(settings->seekThreshold,settings->sksnr,settings->skcnt) ? SKYRADIO_OK : SKYRADIO_ERROR;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRadioSettings( SKYRADIO_SETTINGS *settings )
{
	if(m_radio)
	{
		if(settings)
		{
			m_radio->GetPreferredSeekSettings(settings->seekThreshold,settings->sksnr,settings->skcnt);
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_RaiseVolume()
{
	if(m_radio)
	{
		return m_radio->RaiseVolume() ? SKYRADIO_OK : SKYRADIO_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_LowerVolume()
{
	if(m_radio)
	{
		return m_radio->LowerVolume() ? SKYRADIO_OK : SKYRADIO_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetVolume( unsigned int *volume )
{
	if(m_radio)
	{
		return m_radio->GetVolume(*volume) ? SKYRADIO_OK : SKYRADIO_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_SetVolume( unsigned int volume )
{
	if(m_radio)
	{
		return m_radio->SetVolume(volume) ? SKYRADIO_OK : SKYRADIO_ERROR;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetEONList( SKYRADIO_EON *eonList, size_t *size )
{
	if(m_radio)
	{
		if(size)
		{
			const std::map<unsigned int,CEon> & eonMap = m_radio->GetEONList();
			if(eonMap.size() <= *size)
			{
				*size = eonMap.size();
				size_t i = 0;
				for(auto it = eonMap.cbegin(); it != eonMap.cend(); ++it, ++i)
				{
					eonList[i].pi = it->first;
					eonList[i].ta = it->second.m_ta;
					eonList[i].tp = it->second.m_tp;
					const std::set<unsigned int> & set_afList = it->second.m_af.GetAFList();
					std::copy(set_afList.cbegin(),set_afList.cend(),eonList[i].afList);
					wcsncpy_s(eonList[i].ps,9,it->second.m_ps.GetDisplayString().c_str(),it->second.m_ps.GetDisplayString().length());
					wcsncpy_s(eonList[i].pty,50,it->second.m_pty.GetPtyDisplayString().c_str(),it->second.m_pty.GetPtyDisplayString().length());
				}
				return SKYRADIO_OK;
			}
			else
			{
				*size = eonMap.size();
				return SKYRADIO_INSUFFICIENT_BUFFER;
			}
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetTMCData( SKYRADIO_TMCStruct *tmcStruct )
{
	if(m_radio)
	{
		if(tmcStruct)
		{
			const SKYRADIO_TMCStruct & tmc = m_radio->GetTMCData();
			memcpy(tmcStruct,&tmc,sizeof(SKYRADIO_TMCStruct));
			return SKYRADIO_OK;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}

SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRTPlusContent( SKYRADIO_RTContent *rtStruct, size_t *size )
{
	if(m_radio)
	{
		if(rtStruct && size)
		{
			if(*size >= 2)
			{
				const SKYRADIO_RTContent * ptr = m_radio->GetRTPlusContent();
				memcpy(&rtStruct[0],&ptr[0],sizeof(SKYRADIO_RTContent));
				memcpy(&rtStruct[1],&ptr[1],sizeof(SKYRADIO_RTContent));
				return SKYRADIO_OK;
			}
			return SKYRADIO_INSUFFICIENT_BUFFER;
		}
		return SKYRADIO_NULL_POINTER_PROVIDED;
	}
	return SKYRADIO_NOT_INITIALIZED;
}



