#ifndef RadioRds_h__
#define RadioRds_h__

#include "Enums.h"
#include <map>
#include <vector>
#include <string>
#include "CountryMap.h"
#include "Structures.h"
#include "Pty.h"
#include "Af.h"
#include "Eon.h"
#include "RadioText.h"
#include "StringContructor.h"
#include "TimeDate.h"
#include "BitsManipulator.h"


typedef struct  
{
	BYTE bitsInvolved;
	std::wstring description;
}ldm;



class CRadioRds
{
	typedef void (CRadioRds::*RDSFUNC)(const WORD * _register);
	typedef std::map<RDS_GROUP_TYPE, RDSFUNC> RDSFuncMap;


public:
	CRadioRds(HWND hWnd, const unsigned int userMessage);
	virtual ~CRadioRds(void);


	void UpdateRDS(const WORD * _register);
	void InitRDS(void);
	const std::wstring & GetPtyDisplayString(void) const;
	const std::wstring & GetPtyNDisplayString(void) const;
	const std::wstring & GetPSDisplayString(void) const;
	const std::wstring & GetRegionString(void) const;
	const std::wstring & GetCountryString(void) const;
	const std::wstring & GetLanguageString(void) const;
	const std::wstring & GetRadioTextString(void) const;
	const BYTE * GetPtyType(void);
	const unsigned int GetCurrentPI(void) const;
	const std::set<unsigned int> & GetAFList(void) const;
	const std::map<unsigned int, CEon> & GetEONList(void) const;
	const SKYRADIO_TMCStruct & GetTMCData(void) const;
	const SKYRADIO_RTContent * GetRTPlusContent() const;
	const CTimeDate & GetCurrentRadioTime(void) const;
	const bool GetTAFlag();
	const bool GetTPFlag();
	const bool GetMSFlag();
private:
	std::map<unsigned short, ldm>			m_ldmMap;
	RDSFuncMap								m_RDSFunctionsMap;
	CCountryMap								m_countryMap;

	//verbose mode
	BYTE									m_blerRegisters[4];
	BYTE									m_rdsBlerMax[4];
	BYTE									m_errorFlags;
	bool									m_bA;
	bool									m_bB;
	bool									m_bC;
	bool									m_bD;

	//Handle for TA messages
	HWND									m_hWnd;
	unsigned int							m_userMessage;

	//PI
	WORD									m_piLast;
	int										m_piValidateCounter;
	unsigned short							m_ecc;
	std::wstring							m_piRegionString;
	std::wstring							m_piCountryString;
	std::wstring							m_languageSpoken;

	//PTY
	CPty									m_pty;

	//PTYN
	CStringContructor						m_ptyn;

	//TA & TP
	BYTE									m_ta;
	BYTE									m_tp;
	int										m_taValidateCounter;
	bool									m_trafficNowPlaying;

	//M/S
	BYTE									m_ms;

	// PS
	CStringContructor						m_ps;

	//TMC
	SKYRADIO_TMCStruct								m_TMC;
	CBitsManipulator						m_bits;

	//DATE
	CTimeDate								m_timeDate;

	//AF
	CAf										m_af;

	// PROGRAM ITEM
	BYTE									m_piDay;
	BYTE									m_piHour;
	BYTE									m_piMinute;

	//Radio TEXT
	CRadioText								m_radioText;
	
	//EON
	std::map<unsigned int, CEon>			m_EonList;
	std::map<unsigned int, CEon>			m_tmpEonList;

	//Radio Text PLUS
	BYTE									m_rtPlusTagODA;
	SKYRADIO_RTContent						m_rtplusContent[2];

	//update switch
	__int64									m_startValidTime;

	void InitializeMaps(void);
	void InitializeLabelsDefinitionMap(void);
	void InitializeRDSFunctionsMap(void);

	void CallRDSFunction(const RDS_GROUP_TYPE & groupType, const WORD * _register);

	void RDS_0A_Function(const WORD * _register);
	void RDS_0B_Function(const WORD * _register);
	void RDS_1A_Function(const WORD * _register);
	void RDS_1B_Function(const WORD * _register);
	void RDS_2A_Function(const WORD * _register);
	void RDS_2B_Function(const WORD * _register);
	void RDS_3A_Function(const WORD * _register);
	void RDS_4A_Function(const WORD * _register);
	void RDS_5A_Function(const WORD * _register);
	void RDS_6A_Function(const WORD * _register);
	void RDS_7A_Function(const WORD * _register);
	void RDS_8A_Function(const WORD * _register);
	void RDS_9A_Function(const WORD * _register);
	void RDS_10A_Function(const WORD * _register);
	void RDS_11A_Function(const WORD * _register);
	void RDS_12A_Function(const WORD * _register);
	void RDS_13A_Function(const WORD * _register);
	void RDS_14A_Function(const WORD * _register);
	void RDS_14B_Function(const WORD * _register);
	void RDS_15B_Function(const WORD * _register);

	void UpdateAFList(const WORD * _register);
	void UpdatePI(const WORD currentPi);
	void ValidatePI(const WORD currentPi);
	void ValidateProgramItem(const WORD currentItem);
	void ValidateTP(const WORD currentRegister);
	bool ConsumeTMCBits(bool partial);
	void DecodeRTPlusTag(const WORD * _register);
};
#endif // RadioRds_h__

