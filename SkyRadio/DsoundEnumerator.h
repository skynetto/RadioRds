#ifndef DsoundEnumerator_h__
#define DsoundEnumerator_h__
#include <mmreg.h>
#include <InitGuid.h>
#include <Dsound.h>
#include <dsconf.h>
#include <string>
#pragma comment(lib,"dxguid.lib")

typedef WINUSERAPI HRESULT (WINAPI *LPFNDLLGETCLASSOBJECT) (const CLSID &, const IID &, void **);


typedef struct  
{
	int waveID;
	std::wstring szDescription;
}SKYRADIO_RECORD_ENUM;


class CDsoundEnumerator
{
public:
	CDsoundEnumerator(void);
	virtual ~CDsoundEnumerator(void);

	BOOL GetInfoFromDSoundGUID( GUID i_sGUID, DWORD &dwWaveID, std::wstring & Description);
	HRESULT DSGetGuidFromName ( IN LPCTSTR szName, IN BOOL fRecord, OUT LPGUID pGuid);
	HRESULT DirectSoundPrivateCreate (OUT IKsPropertySet ** ppKsPropertySet);
	static BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext);
	BOOL CALLBACK DSEnumCallbackInternal(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext);
};

#endif // DsoundEnumerator_h__
