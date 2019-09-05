#include "StdAfx.h"
#include "RadioAudioStream.h"
#include <vector>

CRadioAudioStream *CRadioAudioStream::m_ptr(nullptr);

CRadioAudioStream::CRadioAudioStream(void)
	: m_recordChannel(NULL)
	, m_mainWindow(NULL)
	, m_volume(1.0f)
	, m_mute(false)
{
	m_ptr = this;
}


CRadioAudioStream::~CRadioAudioStream(void)
{
	if(m_recordChannel)
	{
		BASS_ChannelStop(m_recordChannel);
		BASS_RecordFree();
	}
	if(m_playChannel)
	{
		BASS_ChannelStop(m_playChannel);
		BASS_StreamFree(m_playChannel); 
	}
	BASS_Free();
	m_ptr = nullptr;
}


bool CRadioAudioStream::Initialize(const HWND hWnd, const std::wstring & deviceName)
{
	m_mainWindow = hWnd;
	if(!BASS_Init(1,FREQUENCY,BASS_DEVICE_3D,m_mainWindow,NULL)) 
		return false;
	BASS_GetInfo(&bi);
	m_playChannel = BASS_StreamCreate(FREQUENCY,2,BASS_SAMPLE_FLOAT,STREAMPROC_PUSH,NULL);
	if(!m_playChannel)
	{
		BASS_StreamFree(m_playChannel); 
		BASS_Free();
		return false;
	}
	
	int input = GetDeviceInputID(deviceName);
	
	if(input != -2)
	{
		BASS_RecordSetDevice(input);
		if(BASS_RecordInit(input))
		{
			m_recordChannel = BASS_RecordStart(FREQUENCY,2,MAKELONG(BASS_SAMPLE_FLOAT,LATENCY),&CRadioAudioStream::RecordingCallback,NULL);
			if(!m_recordChannel)
			{
				BASS_RecordFree();
				BASS_StreamFree(m_playChannel); 
				BASS_Free();
				return false;
			}
		}
		else
		{
			BASS_StreamFree(m_playChannel); 
			BASS_Free();
			return false;
		}
		BASS_ChannelPlay(m_playChannel,FALSE);
		return true;
	}
	else
	{
		BASS_StreamFree(m_playChannel); 
		BASS_Free();
	}
	return false;
}

/*static*/
BOOL CALLBACK CRadioAudioStream::RecordingCallback(HRECORD handle, const void *buffer, DWORD length, void *user)
{
	if(m_ptr != nullptr)
		return m_ptr->RecordingCallbackInternal(handle,buffer,length,user);
	return TRUE;
}

BOOL CALLBACK CRadioAudioStream::RecordingCallbackInternal( HRECORD handle, const void *buffer, DWORD length, void *user )
{
	UNREFERENCED_PARAMETER(handle);
	UNREFERENCED_PARAMETER(user);
	BASS_StreamPutData(m_playChannel,buffer,length); // feed recorded data to output stream
	return TRUE; // continue recording
}

std::wstring CRadioAudioStream::str_to_wstr(const std::string &str, UINT cp)
{
	int len = MultiByteToWideChar(cp, 0, str.c_str(), str.length(), 0, 0);
	if (!len)
		return L"";

	std::vector<wchar_t> wbuff(len + 1);
	// NOTE: this does not NULL terminate the string in wbuff, but this is ok
	//       since it was zero-initialized in the vector constructor
	if (!MultiByteToWideChar(cp, 0, str.c_str(), str.length(), &wbuff[0], len))
		return L"";

	return &wbuff[0];
}//str_to_wstr

std::string CRadioAudioStream::wstr_to_str(const std::wstring &wstr, UINT cp)
{
	int len = WideCharToMultiByte(cp, 0, wstr.c_str(), wstr.length(), 
		0, 0, 0, 0);
	if (!len)
		return "";

	std::vector<char> abuff(len + 1);

	// NOTE: this does not NULL terminate the string in abuff, but this is ok
	//       since it was zero-initialized in the vector constructor
	if (!WideCharToMultiByte(cp, 0, wstr.c_str(), wstr.length(), 
		&abuff[0], len, 0, 0))
	{
		return "";
	}//if

	return &abuff[0];
}//wstr_to_str

bool CRadioAudioStream::MuteStream( const bool & mute )
{
	bool ret;
	if(mute)
	{
		ret = BASS_ChannelSetAttribute(m_playChannel,BASS_ATTRIB_VOL,0) ? true : false;
	}
	else
	{
		ret = BASS_ChannelSetAttribute(m_playChannel,BASS_ATTRIB_VOL,m_volume) ? true : false;
	}
	m_mute = mute;
	return ret;
}

int CRadioAudioStream::GetDeviceInputID( const std::wstring & deviceName )
{
	BASS_DEVICEINFO info;
	int input = -2;
	for (int a = 0; BASS_RecordGetDeviceInfo(a, &info); a++)
	{
		std::wstring str(str_to_wstr(info.name));
		if(str.find(deviceName) != std::wstring::npos)
		{
			input = a;
			break;
		}
	}
	return input;
}

