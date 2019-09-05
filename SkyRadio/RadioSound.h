#ifndef RadioSound_h__
#define RadioSound_h__


#include <atlbase.h>
#include <streams.h>
#include <dshow.h>
#include <d3d9.h>
#include <string>


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

typedef WINUSERAPI HRESULT (WINAPI *LPFNDLLGETCLASSOBJECT) (const CLSID &, const IID &, void **);


class CRadioSound
{
public:
	CRadioSound(void);
	virtual ~CRadioSound(void);
	bool InitializeSound(const std::wstring & deviceName);
	void DestroySound(void);
	bool Mute(const bool & mute);
	bool RaiseVolume(void);
	bool LowerVolume(void);
	bool SetVolume(const unsigned int & volume);
	bool GetVolume(unsigned int & volume);

private:
	HRESULT InitDirectShow();
	HRESULT GetBaseFilter(const PIN_DIRECTION & pin_type, IBaseFilter **filter, const int & waveID);
	HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT SetAudioLatency(IPin *AudioCapturePin, int BufferSizeMilliSeconds);
	int GetDeviceInputID(const std::wstring & deviceName);
	IPin					*OutPin;
	IPin					*InPin;
	IBaseFilter				*dsound;
	IBaseFilter				*radio;
	IGraphBuilder			*g_pGraphBuilder;
	IMediaControl			*g_pMediaControl;
	IBasicAudio				*g_pBasicAudio;	
	ICaptureGraphBuilder2	*g_pCaptureGraphBuilder;
	int						m_currentVolume;
};
#endif // RadioSound_h__

