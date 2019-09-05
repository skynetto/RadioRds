#include "StdAfx.h"
#include "DsoundEnumerator.h"
#include <vector>

static CDsoundEnumerator *m_ptr = NULL;

CDsoundEnumerator::CDsoundEnumerator(void)
{
	m_ptr = this;
}


CDsoundEnumerator::~CDsoundEnumerator(void)
{
}

BOOL CDsoundEnumerator::GetInfoFromDSoundGUID( GUID i_sGUID, DWORD &dwWaveID, std::wstring & Description )
{
	LPKSPROPERTYSET         pKsPropertySet = NULL; 
	HRESULT                 hr; 
	BOOL					retval = FALSE;

	PDSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_DATA psDirectSoundDeviceDescription = NULL; 
	DSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_DATA sDirectSoundDeviceDescription;

	memset(&sDirectSoundDeviceDescription,0,sizeof(sDirectSoundDeviceDescription)); 
	hr = DirectSoundPrivateCreate( &pKsPropertySet ); 
	if(SUCCEEDED(hr)) 
	{ 
		ULONG ulBytesReturned = 0;
		sDirectSoundDeviceDescription.DeviceId = i_sGUID; 

		// On the first call the final size is unknown so pass the size of the struct in order to receive
		// "Type" and "DataFlow" values, ulBytesReturned will be populated with bytes required for struct+strings.
		hr = pKsPropertySet->Get(DSPROPSETID_DirectSoundDevice, 
			DSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION, 
			NULL, 
			0, 
			&sDirectSoundDeviceDescription, 
			sizeof(sDirectSoundDeviceDescription), 
			&ulBytesReturned
			); 

		if (ulBytesReturned)
		{
			// On the first call it notifies us of the required amount of memory in order to receive the strings.
			// Allocate the required memory, the strings will be pointed to the memory space directly after the struct.
			psDirectSoundDeviceDescription = (PDSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_DATA)new BYTE[ulBytesReturned];
			*psDirectSoundDeviceDescription = sDirectSoundDeviceDescription;

			hr = pKsPropertySet->Get(DSPROPSETID_DirectSoundDevice, 
				DSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION, 
				NULL, 
				0, 
				psDirectSoundDeviceDescription, 
				ulBytesReturned, 
				&ulBytesReturned
				); 

			dwWaveID  = psDirectSoundDeviceDescription->WaveDeviceId;
			Description = psDirectSoundDeviceDescription->Description;
			delete [] psDirectSoundDeviceDescription;
			retval = TRUE;
		}

		pKsPropertySet->Release(); 
	} 

	return retval; 
}

HRESULT CDsoundEnumerator::DSGetGuidFromName( IN LPCTSTR szName, IN BOOL fRecord, OUT LPGUID pGuid )
{
	LPKSPROPERTYSET         pKsPropertySet  = NULL; 
	HRESULT                 hr; 

	DSPROPERTY_DIRECTSOUNDDEVICE_WAVEDEVICEMAPPING_DATA WaveDeviceMap = {0}; 

	// Create the DirectSoundPrivate object 
	hr = DirectSoundPrivateCreate ( &pKsPropertySet ); 


	// Attempt to map the waveIn/waveOut device string to a DirectSound device 
	// GUID. 
	if(SUCCEEDED(hr)) 
	{ 
		WaveDeviceMap.DeviceName = (LPTSTR)szName; 
		WaveDeviceMap.DataFlow = fRecord ? DIRECTSOUNDDEVICE_DATAFLOW_CAPTURE : DIRECTSOUNDDEVICE_DATAFLOW_RENDER; 

		hr = pKsPropertySet->Get 
			( 
			DSPROPSETID_DirectSoundDevice, 
			DSPROPERTY_DIRECTSOUNDDEVICE_WAVEDEVICEMAPPING, 
			NULL, 
			0, 
			&WaveDeviceMap, 
			sizeof(WaveDeviceMap), 
			NULL 
			); 
	} 

	// Clean up 
	if(pKsPropertySet) 
	{ 
		pKsPropertySet->Release(); 
	} 


	if(SUCCEEDED(hr)) 
	{ 
		*pGuid = WaveDeviceMap.DeviceId; 
	} 

	return hr; 
}

HRESULT CDsoundEnumerator::DirectSoundPrivateCreate( OUT IKsPropertySet ** ppKsPropertySet )
{
	HMODULE                 hLibDsound              = NULL; 
	LPFNDLLGETCLASSOBJECT   pfnDllGetClassObject    = NULL; 
	LPCLASSFACTORY          pClassFactory           = NULL; 
	LPKSPROPERTYSET         pKsPropertySet          = NULL; 
	HRESULT                 hr                      = DS_OK; 

	// Load dsound.dll 
	hLibDsound = LoadLibrary(TEXT("dsound.dll")); 

	if(!hLibDsound) 
	{ 
		hr = DSERR_GENERIC; 
	} 

	// Find DllGetClassObject 
	if(SUCCEEDED(hr)) 
	{ 
		pfnDllGetClassObject = 
			(LPFNDLLGETCLASSOBJECT)GetProcAddress ( hLibDsound, "DllGetClassObject" ); 


		if(!pfnDllGetClassObject) 
		{ 
			hr = DSERR_GENERIC; 
		} 
	} 

	// Create a class factory object     
	if(SUCCEEDED(hr)) 
	{ 
		hr = pfnDllGetClassObject (CLSID_DirectSoundPrivate, IID_IClassFactory, (LPVOID *)&pClassFactory ); 
	} 

	// Create the DirectSoundPrivate object and query for an IKsPropertySet 
	// interface 
	if(SUCCEEDED(hr)) 
	{ 
		hr = pClassFactory->CreateInstance ( NULL, IID_IKsPropertySet, (LPVOID *)&pKsPropertySet ); 
	} 

	// Release the class factory 
	if(pClassFactory) 
	{ 
		pClassFactory->Release(); 
	} 

	// Handle final success or failure 
	if(SUCCEEDED(hr)) 
	{ 
		*ppKsPropertySet = pKsPropertySet; 
	} 
	else if(pKsPropertySet) 
	{ 
		pKsPropertySet->Release(); 
	} 

	FreeLibrary(hLibDsound); 

	return hr; 
}

BOOL CALLBACK CDsoundEnumerator::DSEnumCallbackInternal( LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext )
{
	LPWSTR psz=NULL;
	StringFromCLSID(*lpGuid, &psz);
	DWORD WaveID = 0xFFFFFFFF;
	std::vector<SKYRADIO_RECORD_ENUM> *ptr = reinterpret_cast<std::vector<SKYRADIO_RECORD_ENUM> *>(lpContext);

	if (lpGuid)
	{
		GUID i_guid = *lpGuid;
		std::wstring sDescription;
		GetInfoFromDSoundGUID(i_guid, WaveID, sDescription);
		SKYRADIO_RECORD_ENUM thisEnum;
		thisEnum.waveID = WaveID;
		thisEnum.szDescription = sDescription;
		ptr->push_back(thisEnum);
	}

	if (psz)
	{
		CoTaskMemFree(psz);
	}

	return TRUE;
}


BOOL CALLBACK CDsoundEnumerator::DSEnumCallback( LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext )
{
	if(m_ptr)
		return m_ptr->DSEnumCallbackInternal(lpGuid,lpcstrDescription,lpcstrModule,lpContext);
	return FALSE;
}
