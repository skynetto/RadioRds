#ifndef RadioAudioStream_h__
#define RadioAudioStream_h__

#include "Bass/bass.h"

#include <string>

#pragma comment(lib,"bass.lib")


#define FREQUENCY	44100
#define LATENCY		450

class CRadioAudioStream
{
public:
	CRadioAudioStream(void);
	virtual ~CRadioAudioStream(void);
	bool Initialize(const HWND hWnd, const std::wstring & deviceName);
	int GetDeviceInputID(const std::wstring & deviceName);
	bool MuteStream(const bool & mute);
private:
	static CRadioAudioStream *m_ptr;
	HRECORD					m_recordChannel;
	HSTREAM					m_playChannel;
	HWND					m_mainWindow;	
	float					m_volume;
	bool					m_mute;
	BASS_INFO				bi;

	static BOOL CALLBACK RecordingCallback(HRECORD handle, const void *buffer, DWORD length, void *user);
	BOOL CALLBACK RecordingCallbackInternal(HRECORD handle, const void *buffer, DWORD length, void *user);

	std::wstring str_to_wstr(const std::string &str, UINT cp = CP_ACP);
	std::string wstr_to_str(const std::wstring &wstr, UINT cp = CP_ACP);

};

#endif // RadioAudioStream_h__
