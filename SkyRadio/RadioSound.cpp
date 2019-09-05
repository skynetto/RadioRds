#include "StdAfx.h"
#include "tchar.h"
#include "RadioSound.h"
#include "DsoundEnumerator.h"
#include <vector>

CRadioSound::CRadioSound(void)
	: OutPin(NULL)
	, InPin(NULL)
	, dsound(NULL)
	, radio(NULL)
	, g_pMediaControl(NULL)
	, g_pBasicAudio(NULL)
	, g_pCaptureGraphBuilder(NULL)
	, g_pGraphBuilder(NULL)
	, m_currentVolume(100)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}


CRadioSound::~CRadioSound(void)
{
	if(g_pMediaControl)
		g_pMediaControl->Stop();
	if(g_pGraphBuilder)
	{
		if(InPin)
			g_pGraphBuilder->Disconnect(InPin);
		if(OutPin)
			g_pGraphBuilder->Disconnect(OutPin);
	}
	if(radio)
		radio->Stop();
	if(dsound)
		dsound->Stop();
	DestroySound();
}

void CRadioSound::DestroySound(void)
{
	SafeRelease(&g_pCaptureGraphBuilder);
	SafeRelease(&g_pGraphBuilder);
	SafeRelease(&g_pMediaControl);
	SafeRelease(&g_pBasicAudio);
	SafeRelease(&dsound);
	SafeRelease(&radio);
	SafeRelease(&InPin);
	SafeRelease(&OutPin);	
	::CoUninitialize();
}

HRESULT CRadioSound::InitDirectShow()
{
	HRESULT hr = E_FAIL;
		
	hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,(void**)&g_pGraphBuilder);

	return hr;
}

bool CRadioSound::InitializeSound(const std::wstring & deviceName)
{
	int waveID = GetDeviceInputID(deviceName);
	if(waveID == -2)
		return false;
	HRESULT hr = InitDirectShow();
	if(FAILED(hr))
	{
		return false;
	}
	hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2, (void**)&g_pCaptureGraphBuilder);
	if(FAILED(hr))	
	{
		return false;
	}

	hr = ::CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&dsound);
	if(dsound != NULL)
		hr = g_pGraphBuilder->AddFilter(dsound, L"Default DirectSound Device");
	else
		return false;
	if(FAILED(hr))
	{
		return false;
	}

	hr = GetBaseFilter(PINDIR_OUTPUT,&radio,waveID);
	if(SUCCEEDED(hr))
	{
		hr = g_pGraphBuilder->AddFilter(radio, L"FM Radio");
		hr = GetPin(radio,PINDIR_OUTPUT,&OutPin);
		hr = GetPin(dsound,PINDIR_INPUT,&InPin);
		if(OutPin != NULL && InPin != NULL)
		{
			SetAudioLatency(OutPin,100);
			hr = g_pGraphBuilder->Connect(OutPin,InPin);
			if(SUCCEEDED(hr))
			{
				hr = g_pCaptureGraphBuilder->SetFiltergraph(g_pGraphBuilder);
				if(SUCCEEDED(hr))
				{
					OutputDebugString(L"Rendered!!\n");
					hr = g_pGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&g_pMediaControl);
					hr = g_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void**)&g_pBasicAudio);
				}
			}
		}
	}
	
	if(g_pMediaControl)
		g_pMediaControl->Run();
	return true;
}

HRESULT CRadioSound::GetBaseFilter( const PIN_DIRECTION & pin_type, IBaseFilter **filter, const int & waveID)
{
	HRESULT hr;
	GUID guidType;
	if(pin_type == PINDIR_INPUT)
	{
		guidType = AM_KSCATEGORY_RENDER;
	}
	else
	{
		guidType = CLSID_AudioInputDeviceCategory;
	}

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if(FAILED(hr))
	{
		return hr;
	}
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(guidType, &pEnumCat, 0);
	if (FAILED(hr)) 
	{
		return hr;
	}
	// Enumerate the monikers.
	IMoniker *pMoniker = NULL;
	bool found = false;
	while(!found && SUCCEEDED(pEnumCat->Next(1, &pMoniker, NULL)))
	{
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
		if (SUCCEEDED(hr))
		{
			IBaseFilter* device;
			hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,(void**)&device);
			if(SUCCEEDED(hr))
			{
				if(guidType == CLSID_AudioInputDeviceCategory)
				{
					VARIANT varName;
					VariantInit(&varName);
					hr = pPropBag->Read(L"WaveInID", &varName, 0);
					if (SUCCEEDED(hr))
					{
						OutputDebugString(L"Found device...");
						// See if this filter is our radio
						if(varName.lVal != waveID)
						{
							OutputDebugString(L"it's not the radio\n");
							device->Release();
						} 
						else 
						{
							OutputDebugString(L"it's the radio\n");
							found = true;
							*filter = device;
						}
					}
					VariantClear(&varName);
				}
				else
				{
					found = true;
					*filter = device;
				}
			}
			pPropBag->Release();
		}
		pMoniker->Release();
	}
	pEnumCat->Release();
	pSysDevEnum->Release();
	return hr;
}

HRESULT CRadioSound::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	IEnumPins  *pEnum = NULL;
	IPin       *pPin = NULL;
	HRESULT    hr;

	if (ppPin == NULL)
	{
		return E_POINTER;
	}

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		hr = pPin->QueryDirection(&PinDirThis);
		if (FAILED(hr))
		{
			pPin->Release();
			pEnum->Release();
			return hr;
		}
		if (PinDir == PinDirThis)
		{
			// Found a match. Return the IPin pointer to the caller.
			*ppPin = pPin;
			pEnum->Release();
			return S_OK;
		}
		// Release the pin for the next time through the loop.
		pPin->Release();
	}
	// No more pins. We did not find a match.
	pEnum->Release();
	return E_FAIL;  
}

bool CRadioSound::Mute( const bool & mute )
{
	if(g_pMediaControl)
	{
		OAFilterState fs;
		g_pMediaControl->GetState(INFINITE,&fs);
		if(fs == State_Running && mute)
		{
			radio->Pause();
			g_pMediaControl->Pause();
		}
		else if(fs == State_Paused && !mute)
		{
			radio->Run(0);
			g_pMediaControl->Run();
		}
		return true;
	}
	return false;
}

HRESULT CRadioSound::SetAudioLatency(IPin *AudioCapturePin, int BufferSizeMilliSeconds)
{
	// Get the interfaces we need.
	CComQIPtr<IAMStreamConfig> streamconfig(AudioCapturePin);
	if (!streamconfig)
		return E_NOINTERFACE; 

	CComQIPtr<IAMBufferNegotiation> bufneg(AudioCapturePin);
	if (!bufneg)
		return E_NOINTERFACE; 

	// Get the current audio capture properties.
	CMediaType mt;
	AM_MEDIA_TYPE *ptr = reinterpret_cast<AM_MEDIA_TYPE*>(&mt);
	HRESULT hr = streamconfig->GetFormat(&ptr);
	if (FAILED(hr))
		return hr; 

	WAVEFORMATEX *wf = (WAVEFORMATEX *)ptr->pbFormat;

	// Set the desired buffer size.
	ALLOCATOR_PROPERTIES props;
	props.cBuffers = -1;
	props.cbBuffer = wf->nAvgBytesPerSec * BufferSizeMilliSeconds/1000;
	props.cbAlign = -1;
	props.cbPrefix = -1;
	return bufneg->SuggestAllocatorProperties(&props);
}

int CRadioSound::GetDeviceInputID( const std::wstring & deviceName )
{
	CDsoundEnumerator m_enumerator;
	std::vector<SKYRADIO_RECORD_ENUM> deviceList;
	DirectSoundCaptureEnumerate(m_enumerator.DSEnumCallback,&deviceList);
	int input = -2;
	for (auto it = deviceList.cbegin(); it != deviceList.cend(); ++it)
	{
		if(it->szDescription.find(deviceName) != std::wstring::npos)
		{
			input = it->waveID;
			break;
		}
	}
	return input;
}

bool CRadioSound::RaiseVolume()
{
	if(m_currentVolume > 10)
		m_currentVolume += 5;
	else
		m_currentVolume++;
	if(m_currentVolume > 100)
		m_currentVolume = 100;
	return SetVolume(m_currentVolume);
}

bool CRadioSound::LowerVolume()
{
	if(m_currentVolume > 10)
		m_currentVolume -= 5;
	else
		m_currentVolume--;
	if(m_currentVolume < 0)
		m_currentVolume = 0;
	return SetVolume(m_currentVolume);

}

bool CRadioSound::SetVolume( const unsigned int & volume )
{
	if(g_pBasicAudio)
	{
		OAFilterState fs;
		LONG m_volume = (volume == 0) ? -10000 : static_cast<int>((log10(volume * 1000.0f) * 2000.0f) - 10000.0f);
		g_pMediaControl->GetState(INFINITE, &fs);
		g_pBasicAudio->put_Volume(m_volume);
		if(volume != 0)
			m_currentVolume = volume;
		return true;
	}
	return false;
}

bool CRadioSound::GetVolume( unsigned int & volume )
{
	volume = m_currentVolume;
	return true;
}
