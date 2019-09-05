#include "StdAfx.h"
#include "RadioDevice.h"
#include <sstream>
#include <devguid.h>
#include <algorithm>
#include <process.h>
#include <fstream>

CRadioDevice::CRadioDevice(HWND hWnd, const unsigned int userMessage, const std::string & settingFileName)
	: m_FMRadioDataHandle(NULL)
	, m_pEndpoint0ReportBuffer(NULL)
	, m_pEndpoint1ReportBuffer(NULL)
	, m_pEndpoint2ReportBuffer(NULL)
	, m_Endpoint0ReportBufferSize(0)
	, m_Endpoint1ReportBufferSize(0)
	, m_Endpoint2ReportBufferSize(0)
	, m_settingsFileName(settingFileName)
	, m_radioDataReaded(false)
	, m_threadShutDownEvent(NULL)
	, m_RDSthreadHandle(NULL)
	, m_pauseProcessing(NULL)
	, m_resumeProcessing(NULL)
	, m_rds(hWnd,userMessage)
	, m_primary(false)
{
	memset(m_Register,0,FMRADIO_REGISTER_NUM);
	std::ifstream m_in(m_settingsFileName.c_str());
	if(m_in.is_open())
	{
		m_in.read(reinterpret_cast<char *>(&m_radioData),sizeof(m_radioData));
		m_radioDataReaded = true;
		m_in.close();
	}
}


CRadioDevice::~CRadioDevice(void)
{
	if(m_threadShutDownEvent)
	{
		::SetEvent(m_threadShutDownEvent);
		if(m_RDSthreadHandle)
		{
			::WaitForSingleObject(m_RDSthreadHandle,INFINITE);
			::CloseHandle(m_RDSthreadHandle);
		}
		::CloseHandle(m_threadShutDownEvent);
	}
	ReleaseResouces();
}

bool CRadioDevice::InitializeRadioDevice(const std::wstring & devicePath)
{
	m_devicePath = devicePath;
	m_primary = devicePath.empty();
	if(OpenRadioDevice())
	{
		if(GetRadioData(&m_radioData))
		{
			if(InitializeRadioData(&m_radioData))
			{
				DoTune(m_radioData.currentStation);
				m_threadShutDownEvent = ::CreateEvent(NULL,TRUE,FALSE,L"ThreadEvent");
				m_pauseProcessing = ::CreateEvent(NULL,TRUE,FALSE,L"PauseThreadEvent");
				m_resumeProcessing = ::CreateEvent(NULL,TRUE,FALSE,L"ResumeThreadEvent");
				if(m_primary) //Main radio device
				{
					m_RDSthreadHandle = (HANDLE)_beginthreadex(NULL,0,&CRadioDevice::RSSIRegisterData,this,0,NULL);
				}//else secondary no thread needed
				Mute(true);
				return true;
			}
		}
	}
	return false;
}

bool CRadioDevice::Seek(const bool & seekUp)
{

	bool status = false;
	bool stc = false, error = false;

	//Set the seekUp bit in the Power Config register
	if (seekUp)
		m_Register[POWERCFG] |= POWERCFG_SEEKUP;
	else
		m_Register[POWERCFG] &= ~POWERCFG_SEEKUP;

	//Set the seek bit in the Power Config register
	m_Register[POWERCFG] |= POWERCFG_SEEK;

	//Use set feature to set the channel
	if (SetRegisterReport(POWERCFG_REPORT, &m_Register[POWERCFG], 1))
	{
		//Read in the status register to poll STC and see when the seek is complete
		
		//Get the current time to check for polling timeout
		SYSTEMTIME systemTime;
		GetSystemTime(&systemTime);
		WORD startTime = systemTime.wSecond + POLL_TIMEOUT_SECONDS;

		do 
		{
			if (GetRegisterReport(STATUSRSSI_REPORT, &m_Register[STATUSRSSI], 1))
			{
				stc = (((m_Register[STATUSRSSI] >> 14) & 0x01) == 0x01);
				error = (((m_Register[STATUSRSSI] >> 13) & 0x01) == 0x01);
			}
			else
			{
				GetSystemTime(&systemTime);
				if ((systemTime.wSecond - startTime) > POLL_TIMEOUT_SECONDS)
					error = true;
			}
		} while (!stc && !error);

		if (stc && !error)
		{

			m_Register[POWERCFG] &= ~POWERCFG_SEEK;
			if (SetRegisterReport(POWERCFG_REPORT, &m_Register[POWERCFG], 1))
				status = true;

			do 
			{
				GetRegisterReport(STATUSRSSI_REPORT, &m_Register[STATUSRSSI], 1);
				stc = (((m_Register[STATUSRSSI] >> 14) & 0x01) == 0x01);
			} while (stc);
			
			GetRegisterReport(READCHAN_REPORT, &m_Register[READCHAN], 1);
			
		}
		else
		{
			m_Register[POWERCFG] &= ~POWERCFG_SEEK;
			SetRegisterReport(POWERCFG_REPORT, &m_Register[POWERCFG], 1);
			status = false;
		}
	}
	else
	{
		//If the write failed, set our seek bit back
		m_Register[POWERCFG] &= ~POWERCFG_SEEK;
		SetRegisterReport(POWERCFG_REPORT, &m_Register[POWERCFG], 1);
		status = false;
	}

	// Save new Freq*/
	m_radioData.currentStation = CalculateStationFrequency(m_Register[READCHAN] & READCHAN_READCHAN);
	m_radioData.signalStrength = m_Register[STATUSRSSI] & STATUSRSSI_RSSI;
	
	m_rds.InitRDS();
	return status;
}

bool CRadioDevice::OpenRadioDevice()
{
	bool status = false;

	HANDLE		hHidDeviceHandle = NULL;
	GUID		hidGuid;
	HDEVINFO	hHidDeviceInfo = NULL;
	int ppid = 0x00;
	int pvid = 0x00;

	//Obtain the HID GUID
	HidD_GetHidGuid(&hidGuid);

	//Use the HID GUID to get a handle to a list of all HID devices connected
	hHidDeviceInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (hHidDeviceInfo != INVALID_HANDLE_VALUE)
	{
		SP_DEVICE_INTERFACE_DATA hidDeviceInterfaceData = {0};
		hidDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		DWORD i = 0;
		BOOL hidResult = 1;

		//Loop through devices until the hidResult fails, the max USB devices are reached, or status is true
		while ((hidResult) && (i < MAX_USB_DEVICES) && (!status))
		{
			//Query the device using the index to get the interface data
			hidResult = SetupDiEnumDeviceInterfaces(hHidDeviceInfo, 0, &hidGuid, i, &hidDeviceInterfaceData);

			//If a successful query was made, use it to get the detailed data of the device
			if (hidResult)
			{
				BOOL detailResult;
				DWORD length, required;
				PSP_DEVICE_INTERFACE_DETAIL_DATA hidDeviceInterfaceDetailData = {0};

				//Obtain the length of the detailed data structure, then allocate space and retrieve it
				SetupDiGetDeviceInterfaceDetail(hHidDeviceInfo, &hidDeviceInterfaceData, NULL, 0, &length, NULL);
				hidDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(length);
				hidDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				detailResult = SetupDiGetDeviceInterfaceDetail(hHidDeviceInfo, &hidDeviceInterfaceData, hidDeviceInterfaceDetailData, length, &required, NULL);

				//If another successful query to the device detail was made, open a handle to
				//determine if the VID and PID are a match as well
				if (detailResult)
				{
					if(m_devicePath != hidDeviceInterfaceDetailData->DevicePath)
					{
						//Open the device				
						hHidDeviceHandle = CreateFile(hidDeviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
						if (hHidDeviceHandle != INVALID_HANDLE_VALUE)
						{
							HIDD_ATTRIBUTES	hidDeviceAttributes;

							//If it is a valid open, then get the attributes of the HID device
							if (HidD_GetAttributes(hHidDeviceHandle, &hidDeviceAttributes))
							{
								//Check that the VID and PID match
								if (((hidDeviceAttributes.VendorID == SILABS_VID) && (hidDeviceAttributes.ProductID == SILABS_PID))
									|| ((hidDeviceAttributes.VendorID == PCEAR_VID) && (hidDeviceAttributes.ProductID == PCEAR_PID))
									|| ((hidDeviceAttributes.VendorID == ADSTECH_VID) && (hidDeviceAttributes.ProductID == ADSTECH_PID))
									)
								{

									if ((hidDeviceAttributes.VendorID == SILABS_VID) && (hidDeviceAttributes.ProductID == SILABS_PID))
									{
										// This is a Silabs reference - decent radio with RDS. We'll use this as primary if
										// we don't already have one
										OutputDebugString(L"Found a SILABS Radio\n");
										if (ppid == 0x00 && pvid == 0x00) 
										{
											// Don't already have a radio, so this will be primary
											pvid = SILABS_VID;
											ppid = SILABS_PID;
											m_FMRadioDataHandle = hHidDeviceHandle;
											m_devicePath = hidDeviceInterfaceDetailData->DevicePath;
										} 
										else if (ppid != SILABS_PID && pvid != SILABS_VID) 
										{
											// Previously found a radio but it's not Silabs reference, 
											// so lets promote this one to primary because it's probably
											// much better
											pvid = SILABS_VID;
											ppid = SILABS_PID;
											if(m_FMRadioDataHandle)
												CloseHandle(m_FMRadioDataHandle);
											m_FMRadioDataHandle = hHidDeviceHandle;
											m_devicePath = hidDeviceInterfaceDetailData->DevicePath;
											OutputDebugString(L" - promoted it to be primary\n");
										}
										// Can't get any better than this, so we can stop here
										status = true;
									} 
									else if ((hidDeviceAttributes.VendorID == ADSTECH_VID) && (hidDeviceAttributes.ProductID == ADSTECH_PID))
									{
										// This is an ADSTECH - probably as good as Silabs reference but there have been some
										// reports of audio interference, so we'll make this 2nd in order of preference
										OutputDebugString(L"Found an ADSTech Radio\n");
										if (ppid == 0x00 && pvid == 0x00) 
										{
											// Don't already have a radio, so this will be primary
											pvid = ADSTECH_VID;
											ppid = ADSTECH_PID;
											m_FMRadioDataHandle = hHidDeviceHandle;
											m_devicePath = hidDeviceInterfaceDetailData->DevicePath;
										} 
										else if (ppid != SILABS_PID && pvid != SILABS_VID) 
										{
											// Previously found a radio but it's not Silabs reference, 
											// so lets promote this one to primary because it's probably
											// much better
											pvid = ADSTECH_VID;
											ppid = ADSTECH_PID;
											if(m_FMRadioDataHandle)
												CloseHandle(m_FMRadioDataHandle);
											m_FMRadioDataHandle = hHidDeviceHandle;
											m_devicePath = hidDeviceInterfaceDetailData->DevicePath;
											OutputDebugString(L" - promoted it to be primary\n");
										}
									} 
									else if ((hidDeviceAttributes.VendorID == PCEAR_VID) && (hidDeviceAttributes.ProductID == PCEAR_PID)) 
									{
										// This is an PCear - it's a piece of shit, but it was dead cheap. It's got audio interference
										// and no RDS. It's only really any good for measuring signal strength as a 2nd radio
										OutputDebugString(L"Found a PCear Radio\n");
										if (ppid == 0x00 && pvid == 0x00) 
										{
											// Don't already have a radio, so this will be primary
											pvid = PCEAR_VID;
											ppid = PCEAR_PID;
											m_FMRadioDataHandle = hHidDeviceHandle;
											m_devicePath = hidDeviceInterfaceDetailData->DevicePath;
										}
									}
								}
								else
								{
									//If they don't match, close the handle and continue the search
									CloseHandle(hHidDeviceHandle);
								}
							}
						}
					}
				}

				//Deallocate space for the detailed data structure
				free(hidDeviceInterfaceDetailData);
			}

			//Increment i for the next device
			i++;
		}
	}

	if(m_FMRadioDataHandle)
	{
		PHIDP_PREPARSED_DATA preparsedData;

		//Get the preparsed data structure
		if (HidD_GetPreparsedData(m_FMRadioDataHandle, &preparsedData))
		{
			HIDP_CAPS capabilities;

			//Used the preparsed data structure to get the device capabilities
			if (HidP_GetCaps(preparsedData, &capabilities))
			{
				//Check that the feature report length is more than 2
				if (capabilities.FeatureReportByteLength > (FMRADIO_REGISTER_NUM * FMRADIO_REGISTER_SIZE))
				{
					//Allocate the right amount of space for the control feature reports (1-17)
					m_Endpoint0ReportBufferSize = capabilities.FeatureReportByteLength;
					m_pEndpoint0ReportBuffer = static_cast<BYTE *>(malloc(m_Endpoint0ReportBufferSize));
					memset(m_pEndpoint0ReportBuffer, 0, m_Endpoint0ReportBufferSize);

					//Allocate the right amount of space for endpoint 1 feature report (18)
					m_Endpoint1ReportBufferSize = RDS_REPORT_SIZE;
					m_pEndpoint1ReportBuffer = static_cast<BYTE *>(malloc(m_Endpoint1ReportBufferSize));
					memset(m_pEndpoint1ReportBuffer, 0, m_Endpoint1ReportBufferSize);

					m_Endpoint2ReportBufferSize = FMTUNERDATA_REPORT_SIZE;
					m_pEndpoint2ReportBuffer = static_cast<BYTE *>(malloc(m_Endpoint2ReportBufferSize));
					memset(m_pEndpoint2ReportBuffer, 0, m_Endpoint2ReportBufferSize);

					if (ReadAllRegisters(m_Register))
					{
						//Set the LED to the connected state
						ChangeLED(CONNECT_STATE);
						status = true;
					}
				}
			}

			//Cleanup the preparsed data
			HidD_FreePreparsedData(preparsedData);
		}
		GetRadioName(m_FMRadioDataHandle,m_deviceName,pvid,ppid);
	}

	return status;
}

const std::wstring & CRadioDevice::GetDeviceName(void) const
{
	return m_deviceName;
}

bool CRadioDevice::ChangeLED(BYTE ledState)
{
	//Set the LED report with the state of the LED
	BYTE ledReport[LED_REPORT_SIZE] = {LED_COMMAND, ledState, 0xFF};


	//Use set report to send the new LED state
	return SetLEDReport(LED_REPORT, ledReport, LED_REPORT_SIZE);
	
}

bool CRadioDevice::ReadAllRegisters(FMRADIO_REGISTER *registers)
{
	return GetRegisterReport(ENTIRE_REPORT, registers, FMRADIO_REGISTER_NUM);

}

bool CRadioDevice::GetRegisterReport(BYTE report, FMRADIO_REGISTER* dataBuffer, DWORD dataBufferSize)
{
	bool status = false;

	//Make sure our handle isn't NULL
	if (m_FMRadioDataHandle)
	{
		//Ensure there will be room in the endpoint buffer for the data requested
		if (dataBufferSize <= ((m_Endpoint0ReportBufferSize - 1) / FMRADIO_REGISTER_SIZE))
		{
			//Check to see if the report to read is a single register, or the entire group, or the RDS data
			if ((report >= DEVICEID_REPORT) && (report <= ENTIRE_REPORT))
			{
				//Clear out the endpoint 0 buffer
				memset(m_pEndpoint0ReportBuffer, 0, m_Endpoint0ReportBufferSize);	

				//Assign the first item in the array to the report number to read
				m_pEndpoint0ReportBuffer[0] = report;

				//Call get feature to get the data
				if (HidD_GetFeature(m_FMRadioDataHandle, m_pEndpoint0ReportBuffer, m_Endpoint0ReportBufferSize))
				{
					//Assign returned data to the dataBuffer
					for (BYTE i = 0; i < dataBufferSize; i++)
					{
						dataBuffer[i] = (m_pEndpoint0ReportBuffer[(i * 2) + 1] << 8) | m_pEndpoint0ReportBuffer[(i * 2) + 2];
					}

					status = true;
				}
			}
			else if (report == RDS_REPORT)
			{
				DWORD bytesRead, Result;

				OVERLAPPED o;
				memset(&o, 0, sizeof(OVERLAPPED));
				o.Offset     = 0; 
				o.OffsetHigh = 0; 
				o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

				//Clear out endpoint 1 buffer
				memset(m_pEndpoint1ReportBuffer, 0, m_Endpoint1ReportBufferSize);

				//Assign the first item in the array to the report number to read
				m_pEndpoint1ReportBuffer[0] = RDS_REPORT;

				//Call a read file on the data handle to read in from endpoint 1
				if (::ReadFile(m_FMRadioDataHandle, m_pEndpoint1ReportBuffer, m_Endpoint1ReportBufferSize, &bytesRead, &o) == FALSE)
				{
					// If it didn't go through, then wait on the object to complete the read
					DWORD error = GetLastError();
					if (error == ERROR_IO_PENDING) 
					{
						do {
							Result = WaitForSingleObject(o.hEvent, 20);
						}while((Result == WAIT_TIMEOUT)/* && !ShouldQuit*/);

						if(GetOverlappedResult(m_FMRadioDataHandle, &o, &bytesRead, false))
							status = true;
					}
				}
				else 
				{
					status = true;
				}

				//Close the object
				CloseHandle(o.hEvent);

				//If the read succeeded, assign returned data to the dataBuffer
				if (status)
					for (BYTE i = 0; i < dataBufferSize; i++)
					{
						dataBuffer[i] = (m_pEndpoint1ReportBuffer[(i * 2) + 1] << 8) | m_pEndpoint1ReportBuffer[(i * 2) + 2];
					}
			}
		}
	}

	return status;
}

bool CRadioDevice::SetRegisterReport(BYTE report, FMRADIO_REGISTER* dataBuffer, DWORD dataBufferSize)
{
	bool status = false;

	//Make sure our handle isn't NULL
	if (m_FMRadioDataHandle)
	{
		//Ensure there will be room in the endpoint buffer for the data requested
		if (dataBufferSize <= ((m_Endpoint0ReportBufferSize - 1) / FMRADIO_REGISTER_SIZE))
		{
			//Check to see that the report to write is a writable register
			if ((report == POWERCFG_REPORT) || (report == CHANNEL_REPORT) ||
				(report == SYSCONFIG1_REPORT) || (report == SYSCONFIG2_REPORT) ||
				(report == SYSCONFIG3_REPORT) || (report == TEST1_REPORT) ||
				(report == TEST2_REPORT) || (report == BOOTCONFIG_REPORT))
			{
				//Clear out the endpoint 0 buffer
				memset(m_pEndpoint0ReportBuffer, 0, m_Endpoint0ReportBufferSize);	

				//Assign the first item in the array to the report number to write
				m_pEndpoint0ReportBuffer[0] = report;

				//Assign the rest of the buffer with the data to write
				for (BYTE i = 0; i < dataBufferSize; i++)
				{
					m_pEndpoint0ReportBuffer[(i * 2) + 1] = (dataBuffer[i] & 0xFF00) >> 8;
					m_pEndpoint0ReportBuffer[(i * 2) + 2] = dataBuffer[i] & 0x00FF;
				}

				//Call set feature to write the data
				if (HidD_SetFeature(m_FMRadioDataHandle, m_pEndpoint0ReportBuffer, m_Endpoint0ReportBufferSize))
					status = true;
			}
		}
	}

	return status;
}

bool CRadioDevice::SetScratchReport(BYTE report, BYTE* dataBuffer, DWORD dataBufferSize)
{
	bool status = false;

	//Make sure our handle isn't NULL
	if (m_FMRadioDataHandle)
	{
		//Ensure there will be room in the endpoint buffer for the data requested
		if (dataBufferSize <= (m_Endpoint0ReportBufferSize - 1))
		{
			//Clear out the endpoint 0 buffer
			memset(m_pEndpoint0ReportBuffer, 0, m_Endpoint0ReportBufferSize);	

			//Assign the first item in the array to the report number to read
			m_pEndpoint0ReportBuffer[0] = report;

			//Assign the rest of the buffer with the data to write
			for (BYTE i = 0; i < dataBufferSize; i++)
			{
				m_pEndpoint0ReportBuffer[i + 1] = dataBuffer[i];
			}

			//Call set feature to write the data
			if (HidD_SetFeature(m_FMRadioDataHandle, m_pEndpoint0ReportBuffer, m_Endpoint0ReportBufferSize))
				status = true;
		}
	}

	return status;
}

bool CRadioDevice::GetScratchReport(BYTE report, BYTE* dataBuffer, DWORD dataBufferSize)
{
	bool status = false;

	//Make sure our handle isn't NULL
	if (m_FMRadioDataHandle)
	{
		//Ensure there will be room in the endpoint buffer for the data requested
		if (dataBufferSize <= (m_Endpoint0ReportBufferSize - 1))
		{
			//Clear out the endpoint 0 buffer
			memset(m_pEndpoint0ReportBuffer, 0, m_Endpoint0ReportBufferSize);	

			//Assign the first item in the array to the report number to read
			m_pEndpoint0ReportBuffer[0] = report;

			//Call get feature to get the data
			if (HidD_GetFeature(m_FMRadioDataHandle, m_pEndpoint0ReportBuffer, m_Endpoint0ReportBufferSize))
			{
				for (BYTE i = 0; i < dataBufferSize; i++)
				{
					dataBuffer[i] = m_pEndpoint0ReportBuffer[i + 1];
				}

				status = true;
			}
		}
	}

	return status;
}

bool CRadioDevice::SetLEDReport(BYTE report, BYTE* dataBuffer, DWORD dataBufferSize)
{
	bool status = false;

	//Make sure our handle isn't NULL
	if (m_FMRadioDataHandle)
	{
		//Ensure there will be room in the endpoint buffer for the data requested
		if (dataBufferSize <= (m_Endpoint0ReportBufferSize - 1))
		{
			//Check to see that the report to write is a writeable register
			if (report == LED_REPORT)
			{
				//Clear out the endpoint 0 buffer
				memset(m_pEndpoint0ReportBuffer, 0, m_Endpoint0ReportBufferSize);	

				//Assign the first item in the array to the report number to write
				m_pEndpoint0ReportBuffer[0] = report;

				//Assign the rest of the buffer with the data to write
				for (BYTE i = 0; i < dataBufferSize; i++)
				{
					m_pEndpoint0ReportBuffer[i + 1] = dataBuffer[i];
				}

				//Call set feature to write the data
				if (HidD_SetFeature(m_FMRadioDataHandle, m_pEndpoint0ReportBuffer, m_Endpoint0ReportBufferSize))
					status = true;
			}
		}
	}

	return status;
}

void CRadioDevice::ReleaseResouces()
{
	if (m_FMRadioDataHandle)
	{
		SetRadioData(&m_radioData);
		std::ofstream m_out(m_settingsFileName.c_str(),std::ios::trunc);
		if(m_out.is_open())
		{
			m_out.write(reinterpret_cast<char *>(&m_radioData),sizeof(m_radioData));
			m_out.close();
		}
		ChangeLED(DISCONNECT_STATE);
		::CloseHandle(m_FMRadioDataHandle);
		m_FMRadioDataHandle = NULL;
	}
	if(m_pEndpoint0ReportBuffer)
	{
		free(m_pEndpoint0ReportBuffer);
		m_pEndpoint0ReportBuffer = NULL;
	}
	if(m_pEndpoint1ReportBuffer)
	{
		free(m_pEndpoint1ReportBuffer);
		m_pEndpoint1ReportBuffer = NULL;
	}
	if(m_pEndpoint2ReportBuffer)
	{
		free(m_pEndpoint2ReportBuffer);
		m_pEndpoint2ReportBuffer = NULL;
	}
}

void CRadioDevice::GetRadioName( HANDLE & m_FMRadioDataHandle, std::wstring & deviceName, int vid, int pid )
{
	if(!m_FMRadioDataHandle)
		return;
	
	const TCHAR sEnumerator[] = (L"USB");   
	HDEVINFO m_hDev;   

	m_hDev = SetupDiGetClassDevs(0, sEnumerator, 0, DIGCF_ALLCLASSES  | DIGCF_PRESENT);   
	if (m_hDev != INVALID_HANDLE_VALUE)   
	{   
		DWORD dwIndex = 0;   
		SP_DEVINFO_DATA devInfo;   
		DWORD BufferSize = 0;   
		DWORD DataType;   
		memset(&devInfo, 0, sizeof(devInfo));   
		devInfo.cbSize = sizeof(devInfo);   
		BYTE Buffer[300];   

		while (SetupDiEnumDeviceInfo(m_hDev, dwIndex ++, &devInfo))   
		{   
			if(SetupDiGetDeviceRegistryProperty(m_hDev,&devInfo,SPDRP_HARDWAREID,&DataType,Buffer,sizeof(Buffer),&BufferSize))   
			{   
				if(IsEqualGUID(GUID_DEVCLASS_MEDIA,devInfo.ClassGuid))
				{
					std::wstring str((wchar_t *)Buffer);
					std::transform(str.begin(),str.end(),str.begin(),toupper);
					std::wstringstream ws;
					ws << std::hex << std::uppercase << vid;
					const std::wstring VID = ws.str();
					ws.str(L"");
					ws << std::hex << std::uppercase << pid;
					const std::wstring PID = ws.str();

					if(str.find(VID) != std::wstring::npos && str.find(PID) != std::wstring::npos)
					{
						BYTE friendlyName[300];
						if(SetupDiGetDeviceRegistryProperty(m_hDev,&devInfo,SPDRP_FRIENDLYNAME ,NULL,friendlyName,sizeof(friendlyName),NULL))   
						{   
							deviceName.assign((wchar_t *)friendlyName);
							break;
						} 
						else
						{
							if(SetupDiGetDeviceRegistryProperty(m_hDev,&devInfo,SPDRP_DEVICEDESC ,NULL,friendlyName,sizeof(friendlyName),NULL))   
							{   
								deviceName.assign((wchar_t *)friendlyName);
								break;
							}  
						}
					}
				}
			}   
		}   
	} 
	SetupDiDestroyDeviceInfoList(m_hDev);
	if((deviceName.find(L"Speakers") != std::wstring::npos) ||
		(deviceName.find(L"USB Audio Device") != std::wstring::npos) ||
		(deviceName.find(L"ADS Instant") != std::wstring::npos))
		deviceName = L"Radio";
}
bool CRadioDevice::GetRadioData(RadioData* radioData)
{
	bool status = false;
	
	if (GetScratchReport(SCRATCH_REPORT, m_ScratchPage, SCRATCH_PAGE_SIZE))
	{	
		//Get all the generic data
		radioData->swVersion = m_ScratchPage[1];
		radioData->hwVersion = m_ScratchPage[2];
		radioData->partNumber = (m_Register[DEVICEID] & DEVICEID_PN) >> 12;
		radioData->manufacturerID = m_Register[DEVICEID] & DEVICEID_MFGID;
		radioData->chipVersion = (m_Register[CHIPID] & CHIPID_REV) >> 10;
		radioData->deviceVersion = (m_Register[CHIPID] & CHIPID_DEV) >> 9;
		radioData->firmwareVersion = static_cast<BYTE>(m_Register[CHIPID] & CHIPID_FIRMWARE);

		//If the scratch page's first byte is 0, then it is new, set all options to default
		if (!m_radioDataReaded)
		{	
			radioData->firstRun = 0;
			if(m_ScratchPage[1] == 0)
				radioData->swVersion = 7;
			if(m_ScratchPage[2] == 0)
				radioData->hwVersion = 1;
			radioData->currentStation = 9960;
			radioData->seekThreshold = PREFERRED_SEEK_THRESHOLD;
			radioData->sksnr = 0x4;
			radioData->skcnt = 0x8;
			radioData->band = DATA_BAND_875_108MHZ;
			radioData->spacing = DATA_SPACING_100KHZ;
			radioData->step = 10;
			radioData->deemphasis = DATA_DEEMPHASIS_50;
			radioData->monoStereo = DATA_MONOSTEREO_STEREO;
			radioData->muteOnStartup = false;
			radioData->scanTime = 4;
			radioData->signalStrength = 0;
		}
		status = true;
	}
	return status;
}


bool CRadioDevice::SetRadioData(RadioData* radioData)
{
	bool status = false;

	//Check that radio data is not NULL
	if (radioData)
	{
		//Clear the entire scratch page to FFs
		memset(m_ScratchPage,0xFF,sizeof(BYTE) * SCRATCH_PAGE_SIZE);
		//for (i = 0; i < SCRATCH_PAGE_SIZE; i++)
		//	m_ScratchPage[i] = 0xFF;

		//Clear the scratch page of used data to 00s
		memset(m_ScratchPage,0x00,sizeof(BYTE) * SCRATCH_PAGE_USED_SIZE);
		//for (i = 0; i < SCRATCH_PAGE_USED_SIZE; i++)
		//	m_ScratchPage[i] = 0x00;

		if(radioData->band == DATA_BAND_875_108MHZ)
		{
			if((radioData->currentStation > 10800) || (radioData->currentStation < 8750))
				radioData->currentStation = 9960;
		}
		else
		{
			if((radioData->currentStation > 9000) || (radioData->currentStation < 7600))
				radioData->currentStation = 8200;
		}


		//See GetRadioData for the format of the scratch page
		m_ScratchPage[0] |= (radioData->firstRun ? 0x00 : 0x01);
		m_ScratchPage[1] |= radioData->swVersion;
		m_ScratchPage[2] |= radioData->hwVersion;
		
		if (SetScratchReport(SCRATCH_REPORT, m_ScratchPage, SCRATCH_PAGE_SIZE))
			status = true;
	}	

	return status;
}


WORD CRadioDevice::CalculateStationFrequency(FMRADIO_REGISTER hexChannel, BYTE ScratchBand, BYTE ScratchSpacing)
{
	WORD band = 8750, spacing = 20;
	
	//Determine the band and spacing
	if(ScratchBand == DATA_BAND_76_90MHZ)
		band = 7600;

	switch(ScratchSpacing)
	{
		case DATA_SPACING_200KHZ :	spacing = 20;	break;
		case DATA_SPACING_100KHZ :	spacing = 10;	break;
		case DATA_SPACING_50KHZ :	spacing = 5;	break;
	}

	return (band + (spacing * hexChannel));
}

WORD CRadioDevice::CalculateStationFrequency(FMRADIO_REGISTER hexChannel)
{
	WORD band = 8750, spacing = 20;

	//Determine the band and spacing
	band = (m_Register[SYSCONFIG2] & SYSCONFIG2_BAND) ? 7600 : 8750;

	switch (m_Register[SYSCONFIG2] & SYSCONFIG2_SPACE)
	{
		case SYSCONFIG2_SPACE_200KHZ :	spacing = 20;	break;
		case SYSCONFIG2_SPACE_100KHZ :	spacing = 10;	break;
		case SYSCONFIG2_SPACE_50KHZ :	spacing = 5;	break;
	}

	return (band + (spacing * hexChannel));
}

WORD CRadioDevice::CalculateStationFrequencyBits(unsigned int frequency)
{
	WORD hexChannel;

	unsigned int band = 8750, spacing = 20;
	
	//Determine the band and spacing
	band = (m_Register[SYSCONFIG2] & SYSCONFIG2_BAND) ? 7600 : 8750;

	switch (m_Register[SYSCONFIG2] & SYSCONFIG2_SPACE)
	{
		case SYSCONFIG2_SPACE_200KHZ :	spacing = 20;	break;
		case SYSCONFIG2_SPACE_100KHZ :	spacing = 10;	break;
		case SYSCONFIG2_SPACE_50KHZ :	spacing = 5;	break;
	}

	hexChannel = static_cast<WORD>((frequency - band) / spacing);

	return hexChannel;
}

bool CRadioDevice::InitializeRadioData(RadioData* radioData)
{
	//Check that radio data is not a NULL pointer
	if (radioData)
	{
		//Set volume to max
		m_Register[SYSCONFIG2] &= ~SYSCONFIG2_VOLUME;
		m_Register[SYSCONFIG2] |= 0xF & SYSCONFIG2_VOLUME;
		
		//Always init the RDBS to be enabled
		m_Register[SYSCONFIG1] &= ~SYSCONFIG1_RDS;
		m_Register[SYSCONFIG1] |= SYSCONFIG1_RDS;

		//Enable RDS verbose mode if firmware >= 15
		if(radioData->firmwareVersion >= 0x0F)
		{
			m_Register[POWERCFG] &= ~POWERCFG_RDSM;
			m_Register[POWERCFG] |= POWERCFG_RDSM;

			m_Register[SYSCONFIG3] &= ~SYSCONFIG3_SKSNR;
			m_Register[SYSCONFIG3] |= radioData->sksnr << 4;

			m_Register[SYSCONFIG3] &= ~SYSCONFIG3_SKCNT;
			m_Register[SYSCONFIG3] |= radioData->skcnt;
		}
		else
		{
			//m_Register[POWERCFG] &= ~POWERCFG_RDSM;

			m_Register[SYSCONFIG3] &= ~SYSCONFIG3_SKSNR;
			m_Register[SYSCONFIG3] |= 0 << 4;

			m_Register[SYSCONFIG3] &= ~SYSCONFIG3_SKCNT;
			m_Register[SYSCONFIG3] |= 0;
		}

		//Check the band
		switch (radioData->band & DATA_BAND)
		{
			case DATA_BAND_875_108MHZ	:	m_Register[SYSCONFIG2] &= ~SYSCONFIG2_BAND;	break;
			case DATA_BAND_76_90MHZ		:	m_Register[SYSCONFIG2] |= SYSCONFIG2_BAND;	break;
			default						:	m_Register[SYSCONFIG2] &= ~SYSCONFIG2_BAND;	break;
		}

		//Check the spacing
		m_Register[SYSCONFIG2] &= ~SYSCONFIG2_SPACE;
		switch (radioData->spacing & DATA_SPACING)
		{
			case DATA_SPACING_200KHZ	:	m_Register[SYSCONFIG2] |= SYSCONFIG2_SPACE_200KHZ;	radioData->step = 20; break;
			case DATA_SPACING_100KHZ	:	m_Register[SYSCONFIG2] |= SYSCONFIG2_SPACE_100KHZ;	radioData->step = 10; break;
			case DATA_SPACING_50KHZ		:	m_Register[SYSCONFIG2] |= SYSCONFIG2_SPACE_50KHZ;	radioData->step = 5; break;
			default						:	m_Register[SYSCONFIG2] |= SYSCONFIG2_SPACE_200KHZ;	radioData->step = 20; break;
		}

		//Check the de-emphasis
		switch (radioData->deemphasis & DATA_DEEMPHASIS)
		{
			case DATA_DEEMPHASIS_75		:	m_Register[SYSCONFIG1] &= ~SYSCONFIG1_DE;	break;
			case DATA_DEEMPHASIS_50		:	m_Register[SYSCONFIG1] |= SYSCONFIG1_DE;	break;
			default						:	m_Register[SYSCONFIG1] &= ~SYSCONFIG1_DE;	break;
		}

		//Check mono/stereo selection
		switch (radioData->monoStereo & DATA_MONOSTEREO)
		{
			case DATA_MONOSTEREO_STEREO	:	m_Register[POWERCFG] &= ~POWERCFG_MONO;	break;
			case DATA_MONOSTEREO_MONO	:	m_Register[POWERCFG] |= POWERCFG_MONO;	break;
			default						:	m_Register[POWERCFG] &= ~POWERCFG_MONO;	break;
		}

		if(radioData->muteOnStartup)
			m_Register[POWERCFG] &= ~POWERCFG_DMUTE;
		else
			m_Register[POWERCFG] |= POWERCFG_DMUTE;


		//Check the seek threshold
		m_Register[SYSCONFIG2] &= ~SYSCONFIG2_SEEKTH;
		m_Register[SYSCONFIG2] |= static_cast<WORD>(radioData->seekThreshold) << 8;
				
		//Write the registers
		return (WriteRegister(SYSCONFIG1_REPORT, m_Register[SYSCONFIG1]) &&
			WriteRegister(SYSCONFIG2_REPORT, m_Register[SYSCONFIG2]) &&
			WriteRegister(SYSCONFIG3_REPORT, m_Register[SYSCONFIG3])) &&
			WriteRegister(POWERCFG_REPORT, m_Register[POWERCFG]);
	}

	return false;
}

bool CRadioDevice::WriteRegister(BYTE report, FMRADIO_REGISTER registers)
{
	m_Register[report - 1] = registers;

	//Write the report submitted
	return SetRegisterReport(report, &m_Register[report - 1], FMRADIO_REGISTER_NUM);
}

bool CRadioDevice::Mute(const bool & mute)
{

	if (mute)
	{
		m_Register[POWERCFG] &= ~POWERCFG_DMUTE;
	}
	else
	{
		m_Register[POWERCFG] |= POWERCFG_DMUTE;
	}

	if (SetRegisterReport(POWERCFG_REPORT, &m_Register[POWERCFG], 1))
		return true;
	else
		return false;
}

const unsigned int CRadioDevice::GetCurrentStation() const
{
	return m_radioData.currentStation;
}

bool CRadioDevice::Tune( const bool & tuneUp )
{
	if(tuneUp)
		return DoTune(m_radioData.currentStation + m_radioData.step);
	else
		return DoTune(m_radioData.currentStation - m_radioData.step);
}


bool CRadioDevice::SetFrequency( const unsigned int & frequency )
{
	return DoTune(frequency);
}

bool CRadioDevice::DoTune(const unsigned int & frequency)
{
	bool status = false;

	//Check that the frequency is in range for the current band
	if (((m_Register[SYSCONFIG2] & SYSCONFIG2_BAND) && (frequency >= 7600) && (frequency <= 9000)) || 
		(!(m_Register[SYSCONFIG2] & SYSCONFIG2_BAND) && (frequency >= 8750) && (frequency <= 10800)))
	{
		//Determine the frequency bits
		WORD channel = CalculateStationFrequencyBits(frequency);

		m_Register[CHANNEL] = channel | CHANNEL_TUNE;

		//Use set feature to set the channel
		if (SetRegisterReport(CHANNEL_REPORT, &m_Register[CHANNEL], 1))
		{
			//Read in the status register to poll STC and see when the tune is complete
			bool stc = false, error = false;

			//Get the current time to check for polling timeout
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			WORD startTime = systemTime.wSecond + POLL_TIMEOUT_SECONDS;

			//Poll the RSSI register to see if STC gets set
			do 
			{
				if (GetRegisterReport(STATUSRSSI_REPORT, &m_Register[STATUSRSSI], 1))
				{
					stc = (((m_Register[STATUSRSSI] >> 14) & 0x01) == 0x01);
					error = (((m_Register[STATUSRSSI] >> 13) & 0x01) == 0x01);
				}
				else
				{
					GetSystemTime(&systemTime);
					if ((systemTime.wSecond - startTime) > POLL_TIMEOUT_SECONDS)
						error = true;
				}
			} while (!stc && !error);

			//Once we are out of the polling loop, if there was no error and tune completed, clear 
			//the channel bit and get the current channel
			if (stc && !error)
			{
				m_Register[CHANNEL] &= ~CHANNEL_TUNE;

				if (SetRegisterReport(CHANNEL_REPORT, &m_Register[CHANNEL], 1))
					status = true;

				//wait for STC to be set
				do 
				{
					GetRegisterReport(STATUSRSSI_REPORT, &m_Register[STATUSRSSI], 1);
					stc = (((m_Register[STATUSRSSI] >> 14) & 0x01) == 0x01);
				} while (stc);

				GetRegisterReport(READCHAN_REPORT, &m_Register[READCHAN], 1);

			}
		}
		else
		{
			//If the write failed, set our tune bit back
			m_Register[CHANNEL] &= ~CHANNEL_TUNE;
			SetRegisterReport(CHANNEL_REPORT, &m_Register[CHANNEL], 1);
		}

	}	
	m_radioData.currentStation = CalculateStationFrequency(m_Register[READCHAN] & READCHAN_READCHAN);
	m_radioData.signalStrength = m_Register[STATUSRSSI] & STATUSRSSI_RSSI;
	m_rds.InitRDS();
	return status;
}

const unsigned int CRadioDevice::GetSignalStrength()
{
	GetRegisterReport(RDS_REPORT, &m_Register[STATUSRSSI], RDS_REGISTER_NUM);
	m_radioData.signalStrength = m_Register[STATUSRSSI] & STATUSRSSI_RSSI;
	return m_radioData.signalStrength;
}

unsigned int __stdcall CRadioDevice::RSSIRegisterData( void *lParam )
{
	CRadioDevice *ptr = reinterpret_cast<CRadioDevice *>(lParam);
	return ptr->RSSIRegisterDataInternal();
}

unsigned int CRadioDevice::RSSIRegisterDataInternal()
{
	DWORD result = WAIT_TIMEOUT;
	volatile bool running = true;
	HANDLE allEvents[3] = {m_threadShutDownEvent, m_resumeProcessing, m_pauseProcessing};
	while(running)
	{
		//RDS data updated each 40ms
		result = ::WaitForMultipleObjects(3,allEvents,FALSE,40);
		switch(result)
		{
			default: 
			case WAIT_OBJECT_0: running = false; break;
			case WAIT_OBJECT_0 + 1:
			case WAIT_TIMEOUT: 
			{
				GetRegisterReport(RDS_REPORT, &m_Register[STATUSRSSI], RDS_REGISTER_NUM);
				m_radioData.signalStrength = m_Register[STATUSRSSI] & STATUSRSSI_RSSI;
				if(m_Register[POWERCFG] & POWERCFG_RDSM)
				{
					if((m_Register[STATUSRSSI] >> 15) & 0x01) //RDSR
						m_rds.UpdateRDS(m_Register);
				}
				else
				{
					m_rds.UpdateRDS(m_Register);
				}
			}
			break;
			case WAIT_OBJECT_0 + 2: break;
		}
	}
	return 0L;
}

const std::wstring & CRadioDevice::GetPtyDisplayString() const
{
	return m_rds.GetPtyDisplayString();
}

const bool CRadioDevice::GetTAFlag()
{
	return m_rds.GetTAFlag();
}

const bool CRadioDevice::GetTPFlag()
{
	return m_rds.GetTPFlag();
}

const std::wstring & CRadioDevice::GetPSDisplayString() const
{
	return m_rds.GetPSDisplayString();
}

const bool CRadioDevice::GetMSFlag()
{
	return m_rds.GetMSFlag();
}

const std::set<unsigned int> & CRadioDevice::GetAFList() const
{
	return m_rds.GetAFList();
}

const std::wstring & CRadioDevice::GetPtyNDisplayString() const
{
	return m_rds.GetPtyNDisplayString();
}

const std::wstring & CRadioDevice::GetDevicePath() const
{
	return m_devicePath;
}

const bool CRadioDevice::StartAFSearch( const unsigned int frequency, const std::set<unsigned int> freqAFList )
{
	unsigned int lastStrength = GetSignalStrength();
	const unsigned int lastPi = GetCurrentStationPI();
	unsigned int bestFrequency(0);
	bool found = false;
	Mute(true);
	for(auto it = freqAFList.cbegin(); it != freqAFList.cend(); ++it)
	{
		PauseRDSProcessing();
		if(*it != frequency)
		{
			if(DoTune(*it))
			{
				Sleep(500);
				const unsigned int strength = GetSignalStrength();
				if(strength > lastStrength)
				{
					ResumeRDSProcessing();
					Sleep(500);
					const unsigned int pi = GetCurrentStationPI();
#ifdef _DEBUG
					std::wstringstream ss;
					ss << L"PI : " << pi << L" - freq : " << *it << L" - Strength : " << strength << std::endl;
					OutputDebugString(ss.str().c_str());
#endif
					if(GetStereoFlag())
					{
						if(pi == lastPi)
						{
							found = true;
							lastStrength = strength;
							bestFrequency = *it;
						}
					}
				}
			}
		}
	}
	if(found)
	{
		DoTune(bestFrequency);
	}
	else
	{
		DoTune(frequency);
	}
	ResumeRDSProcessing();
	Mute(false);
	return found;
}

const std::wstring & CRadioDevice::GetRegionString() const
{
	return m_rds.GetRegionString();
}

const std::wstring & CRadioDevice::GetCountryString() const
{
	return m_rds.GetCountryString();
}

const unsigned int CRadioDevice::GetCurrentStationPI() const
{
	return m_rds.GetCurrentPI();
}

const std::wstring & CRadioDevice::GetLanguageString() const
{
	return m_rds.GetLanguageString();
}

const CTimeDate & CRadioDevice::GetCurrentRadioTime() const
{
	return m_rds.GetCurrentRadioTime();
}

const BYTE * CRadioDevice::GetPtyType()
{
	return m_rds.GetPtyType();
}

const std::wstring & CRadioDevice::GetRadioTextString() const
{
	return m_rds.GetRadioTextString();
}

bool CRadioDevice::SetPreferredSeekSettings( const BYTE seekThreshold, const BYTE sksnr, const BYTE skcnt )
{
	m_radioData.seekThreshold = seekThreshold;
	m_radioData.skcnt = skcnt;
	m_radioData.sksnr = sksnr;

	//Check the seek threshold
	m_Register[SYSCONFIG2] &= ~SYSCONFIG2_SEEKTH;
	m_Register[SYSCONFIG2] |= static_cast<WORD>(m_radioData.seekThreshold) << 8;

	//Enable RDS verbose mode if firmware >= 15
	if(m_radioData.firmwareVersion >= 0x0F)
	{
		m_Register[SYSCONFIG3] &= ~SYSCONFIG3_SKSNR;
		m_Register[SYSCONFIG3] |= m_radioData.sksnr << 4;

		m_Register[SYSCONFIG3] &= ~SYSCONFIG3_SKCNT;
		m_Register[SYSCONFIG3] |= m_radioData.skcnt;

		return (WriteRegister(SYSCONFIG2_REPORT, m_Register[SYSCONFIG2])) &&
			(WriteRegister(SYSCONFIG3_REPORT, m_Register[SYSCONFIG3]));
	}
	else
	{
		return WriteRegister(SYSCONFIG2_REPORT, m_Register[SYSCONFIG2]);
	}
}

void CRadioDevice::GetPreferredSeekSettings( BYTE & seekThreshold, BYTE & sksnr, BYTE & skcnt )
{
	seekThreshold = m_radioData.seekThreshold;
	sksnr = m_radioData.sksnr;
	skcnt = m_radioData.skcnt;
}

const bool CRadioDevice::GetStereoFlag()
{
	return (((m_Register[STATUSRSSI] >> 8) & 0x01) == 0x01);
}

const std::map<unsigned int, CEon> & CRadioDevice::GetEONList() const
{
	return m_rds.GetEONList();
}

const SKYRADIO_TMCStruct & CRadioDevice::GetTMCData() const
{
	return m_rds.GetTMCData();
}

const bool CRadioDevice::IsCurrentStationValid()
{
	GetRegisterReport(RDS_REPORT, &m_Register[STATUSRSSI], RDS_REGISTER_NUM);
	return (((m_Register[STATUSRSSI] >> 12) & 0x01) == 0);
}

void CRadioDevice::PauseRDSProcessing()
{
	if(m_resumeProcessing)
		::ResetEvent(m_resumeProcessing);
	if(m_pauseProcessing)
		::SetEvent(m_pauseProcessing);
}

void CRadioDevice::ResumeRDSProcessing()
{
	if(m_pauseProcessing)
		::ResetEvent(m_pauseProcessing);
	if(m_resumeProcessing)
		::SetEvent(m_resumeProcessing);
}

const SKYRADIO_RTContent * CRadioDevice::GetRTPlusContent() const
{
	return m_rds.GetRTPlusContent();
}
