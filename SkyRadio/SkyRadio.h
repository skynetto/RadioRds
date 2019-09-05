#ifndef SkyRadio_h__
#define SkyRadio_h__

#include "RadioErrorsEnum.h"
#include "Structures.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SKYRADIO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SKYRADIO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef SKYRADIO_EXPORTS
#define SKYRADIO_API __declspec(dllexport)
#else
#define SKYRADIO_API __declspec(dllimport) 
#endif

extern "C"
{
	SKYRADIO_API typedef struct  
	{
		BYTE seekThreshold;
		BYTE sksnr;
		BYTE skcnt;
	}SKYRADIO_SETTINGS;

	SKYRADIO_API typedef struct  
	{
		BYTE ta;
		BYTE tp;
		unsigned int pi;
		wchar_t ps[9];
		wchar_t pty[50];
		unsigned int afList[25];
	}SKYRADIO_EON;

	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetAFList(unsigned int *afList, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCountryString(wchar_t * countryString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCurrentRadioTime(int *year, int *month, int *day, int *hour, int *minute);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCurrentStation(unsigned int * currentFreq);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetCurrentStationPI(unsigned int * currentPI);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetLanguageString(wchar_t * languageString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetMSFlag(bool * msFlag);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPSString(wchar_t * ptyString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPTYNString(wchar_t * ptynString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPTYString(wchar_t * ptyString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetPTYType(unsigned char * ptyType);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetEONList(SKYRADIO_EON *eonList, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetTMCData(SKYRADIO_TMCStruct *tmcStruct);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRTPlusContent(SKYRADIO_RTContent *rtStruct, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRadioSettings(SKYRADIO_SETTINGS *settings);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRadioTextString(wchar_t * radioTextString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetRegionString(wchar_t * regionString, size_t *size);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetSignalStrength(unsigned int * rssi);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetTAFlag(bool * taFlag);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetStereoFlag(bool * stereoFlag);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetTPFlag(bool * tpFlag);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_GetVolume(unsigned int *volume);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_LowerVolume();
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_Mute(const bool mute);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_RaiseVolume();
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_Seek(const bool seekUp);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_SetCurrentStation(const unsigned int currentFreq);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_SetRadioSettings(SKYRADIO_SETTINGS *settings);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_SetVolume(unsigned int volume);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_Tune(const bool tuneUp);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_TurnOff(void);
	SKYRADIO_API SKYRADIO_RESULT WINAPI SkyRadio_TurnOn(HWND hWnd, const unsigned int userMessage);
};
#endif // SkyRadio_h__