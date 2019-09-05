#ifndef RadioDevice_h__
#define RadioDevice_h__

extern "C"
{
#include "hid/hidsdi.h"
};

#include <setupapi.h>
#include "Enums.h"
#include "RadioRds.h"
#include <string>

#pragma comment (lib, "hid.lib")
#pragma comment (lib, "setupapi.lib")

typedef struct RadioData
{
	BYTE firstRun;
	BYTE swVersion;
	BYTE hwVersion;
	BYTE partNumber;
	WORD manufacturerID;
	BYTE chipVersion;
	BYTE deviceVersion;
	BYTE firmwareVersion;
	WORD currentStation;
	BYTE seekThreshold;
	BYTE sksnr;
	BYTE skcnt;
	BYTE band;
	BYTE spacing;
	unsigned int step;
	BYTE deemphasis;
	BYTE monoStereo;
	BYTE muteOnStartup;
	BYTE scanTime;
	WORD signalStrength;
} RadioData;

class CRadioDevice
{
public:
	CRadioDevice(HWND hWnd, const unsigned int userMessage,const std::string & settingsfileName);
	virtual ~CRadioDevice(void);
	bool InitializeRadioDevice(const std::wstring & devicePath);
	const std::wstring & GetDeviceName(void) const;
	const std::wstring & GetDevicePath(void) const;
	bool Seek(const bool & seekUp);
	bool Tune(const bool & tuneUp);
	bool SetFrequency(const unsigned int & frequency);
	bool Mute(const bool & mute);
	bool SetPreferredSeekSettings(const BYTE seekThreshold, const BYTE sksnr, const BYTE skcnt);
	void GetPreferredSeekSettings(BYTE & seekThreshold, BYTE & sksnr, BYTE & skcnt);
	const unsigned int GetCurrentStation(void) const;
	const unsigned int GetCurrentStationPI(void) const;
	const unsigned int GetSignalStrength(void);
	const std::wstring & GetPtyDisplayString(void) const;
	const std::wstring & GetPtyNDisplayString(void) const;
	const std::wstring & GetPSDisplayString(void) const;
	const std::wstring & GetRegionString(void) const;
	const std::wstring & GetCountryString(void) const;
	const std::wstring & GetLanguageString(void) const;
	const std::wstring & GetRadioTextString(void) const;
	const SKYRADIO_TMCStruct & GetTMCData(void) const;
	const SKYRADIO_RTContent * GetRTPlusContent() const;
	const std::map<unsigned int, CEon> & GetEONList(void) const;
	const BYTE * GetPtyType(void);
	const std::set<unsigned int> & GetAFList(void) const;
	const CTimeDate & GetCurrentRadioTime(void) const;
	const bool GetTAFlag();
	const bool GetTPFlag();
	const bool GetMSFlag();
	const bool GetStereoFlag();
	const bool StartAFSearch(const unsigned int frequency, const std::set<unsigned int> freqAFList);
	void PauseRDSProcessing(void);
	void ResumeRDSProcessing(void);
private:

	HANDLE				m_FMRadioDataHandle;
	RadioData			m_radioData;
	bool				m_radioDataReaded;
	bool				m_primary;

	BYTE*	m_pEndpoint0ReportBuffer;
	DWORD	m_Endpoint0ReportBufferSize;

	BYTE*	m_pEndpoint1ReportBuffer;
	DWORD	m_Endpoint1ReportBufferSize;

	BYTE*	m_pEndpoint2ReportBuffer;
	DWORD	m_Endpoint2ReportBufferSize;

	FMRADIO_REGISTER	m_Register[FMRADIO_REGISTER_NUM];
	SCRATCH_PAGE		m_ScratchPage;

	std::wstring		m_deviceName;
	std::wstring		m_devicePath;
	std::string			m_settingsFileName;
	
	CRadioRds			m_rds;

	// Thread Communication Handles
	HANDLE				m_RDSthreadHandle;
	HANDLE				m_threadShutDownEvent;
	HANDLE				m_pauseProcessing;
	HANDLE				m_resumeProcessing;

	WORD CalculateStationFrequencyBits(unsigned int frequency);
	bool ChangeLED(BYTE ledState);
	bool DoTune(const unsigned int & frequency);
	bool GetRadioData(RadioData* radioData);
	bool GetRegisterReport(BYTE report, FMRADIO_REGISTER* dataBuffer, DWORD dataBufferSize);
	bool GetScratchReport(BYTE report, BYTE* dataBuffer, DWORD dataBufferSize);
	bool InitializeRadioData(RadioData* radioData);
	bool OpenRadioDevice();
	bool ReadAllRegisters(FMRADIO_REGISTER *registers);
	bool SetLEDReport(BYTE report, BYTE* dataBuffer, DWORD dataBufferSize);
	bool SetRadioData(RadioData* radioData);
	bool SetRegisterReport(BYTE report, FMRADIO_REGISTER* dataBuffer, DWORD dataBufferSize);
	bool SetScratchReport(BYTE report, BYTE* dataBuffer, DWORD dataBufferSize);
	const bool IsCurrentStationValid(void);
	bool WriteRegister(BYTE report, FMRADIO_REGISTER registers);
	WORD CalculateStationFrequency(FMRADIO_REGISTER hexChannel);
	WORD CalculateStationFrequency(FMRADIO_REGISTER hexChannel, BYTE ScratchBand, BYTE ScratchSpacing);
	void GetRadioName(HANDLE & m_FMRadioDataHandle, std::wstring & deviceName, int vid, int pid);
	void ReleaseResouces(void);


	static unsigned int __stdcall RSSIRegisterData(void  *lParam);
	unsigned int RSSIRegisterDataInternal(void);
};


#endif // RadioDevice_h__
