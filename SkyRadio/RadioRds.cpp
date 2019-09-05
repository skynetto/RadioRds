#include "StdAfx.h"
#include "RadioRds.h"
#include <iostream>
#ifdef _DEBUG
#include <sstream>
#endif

CRadioRds::CRadioRds(HWND hWnd, const unsigned int userMessage)
	: m_hWnd(hWnd)
	, m_userMessage(userMessage)
	, m_bA(true)
	, m_bB(true)
	, m_bC(true)
	, m_bD(true)
	, m_ptyn(PTYNSIZE,PTYN_VALIDATE_LIMIT)
	, m_ps(PSSIZE, PS_VALIDATE_LIMIT)
{
	m_rdsBlerMax[0] = CORRECTED_THREE_TO_FIVE; // Block A
	m_rdsBlerMax[1] = CORRECTED_ONE_TO_TWO; // Block B
	m_rdsBlerMax[2] = CORRECTED_THREE_TO_FIVE; // Block C
	m_rdsBlerMax[3] = CORRECTED_THREE_TO_FIVE; // Block D

	InitRDS();
	InitializeMaps();
}


CRadioRds::~CRadioRds(void)
{
}

void CRadioRds::InitializeMaps()
{
	InitializeRDSFunctionsMap();
	InitializeLabelsDefinitionMap();
}

void CRadioRds::InitializeLabelsDefinitionMap()
{
	ldm thisLdm;

	thisLdm.bitsInvolved = 3;
	thisLdm.description = L"Duration";
	m_ldmMap[0] = thisLdm;

	thisLdm.bitsInvolved = 3;
	thisLdm.description = L"Control Code";
	m_ldmMap[1] = thisLdm;

	thisLdm.bitsInvolved = 5;
	thisLdm.description = L"Length";
	m_ldmMap[2] = thisLdm;

	thisLdm.bitsInvolved = 5;
	thisLdm.description = L"Speed Limit";
	m_ldmMap[3] = thisLdm;

	thisLdm.bitsInvolved = 5;
	thisLdm.description = L"Quantifier";
	m_ldmMap[4] = thisLdm;

	thisLdm.bitsInvolved = 8;
	thisLdm.description = L"Quantifier";
	m_ldmMap[5] = thisLdm;

	thisLdm.bitsInvolved = 8;
	thisLdm.description = L"Info Code";
	m_ldmMap[6] = thisLdm;

	thisLdm.bitsInvolved = 8;
	thisLdm.description = L"Start Time";
	m_ldmMap[7] = thisLdm;

	thisLdm.bitsInvolved = 8;
	thisLdm.description = L"Stop Time";
	m_ldmMap[8] = thisLdm;

	thisLdm.bitsInvolved = 11;
	thisLdm.description = L"Event";
	m_ldmMap[9] = thisLdm;

	thisLdm.bitsInvolved = 16;
	thisLdm.description = L"Diversion";
	m_ldmMap[10] = thisLdm;

	thisLdm.bitsInvolved = 16;
	thisLdm.description = L"Destination";
	m_ldmMap[11] = thisLdm;

	thisLdm.bitsInvolved = 16;
	thisLdm.description = L"Reserved";
	m_ldmMap[12] = thisLdm;

	thisLdm.bitsInvolved = 16;
	thisLdm.description = L"Cross linkage";
	m_ldmMap[13] = thisLdm;

	thisLdm.bitsInvolved = 0;
	thisLdm.description = L"Content Separator";
	m_ldmMap[14] = thisLdm;

	thisLdm.bitsInvolved = 0;
	thisLdm.description = L"Reserved";
	m_ldmMap[15] = thisLdm;
}

const std::wstring & CRadioRds::GetPtyDisplayString() const
{
	return m_pty.GetPtyDisplayString();
}

void CRadioRds::InitializeRDSFunctionsMap()
{
	m_RDSFunctionsMap[RDS_0A] = &CRadioRds::RDS_0A_Function;
	m_RDSFunctionsMap[RDS_0B] = &CRadioRds::RDS_0B_Function;
	m_RDSFunctionsMap[RDS_1A] = &CRadioRds::RDS_1A_Function;
	m_RDSFunctionsMap[RDS_1B] = &CRadioRds::RDS_1B_Function;
	m_RDSFunctionsMap[RDS_2A] = &CRadioRds::RDS_2A_Function;
	m_RDSFunctionsMap[RDS_2B] = &CRadioRds::RDS_2B_Function;
	m_RDSFunctionsMap[RDS_3A] = &CRadioRds::RDS_3A_Function;
	m_RDSFunctionsMap[RDS_4A] = &CRadioRds::RDS_4A_Function;
	m_RDSFunctionsMap[RDS_5A] = &CRadioRds::RDS_5A_Function;
	m_RDSFunctionsMap[RDS_6A] = &CRadioRds::RDS_6A_Function;
	m_RDSFunctionsMap[RDS_7A] = &CRadioRds::RDS_7A_Function;
	m_RDSFunctionsMap[RDS_8A] = &CRadioRds::RDS_8A_Function;
	m_RDSFunctionsMap[RDS_9A] = &CRadioRds::RDS_9A_Function;
	m_RDSFunctionsMap[RDS_10A] = &CRadioRds::RDS_10A_Function;
	m_RDSFunctionsMap[RDS_11A] = &CRadioRds::RDS_11A_Function;
	m_RDSFunctionsMap[RDS_12A] = &CRadioRds::RDS_12A_Function;
	m_RDSFunctionsMap[RDS_13A] = &CRadioRds::RDS_13A_Function;
	m_RDSFunctionsMap[RDS_14A] = &CRadioRds::RDS_14A_Function;
	m_RDSFunctionsMap[RDS_14B] = &CRadioRds::RDS_14B_Function;
	m_RDSFunctionsMap[RDS_15B] = &CRadioRds::RDS_15B_Function;
}

void CRadioRds::RDS_0A_Function( const WORD * _register )
{
	m_ms = ((_register[RDSB] >> 3) & 0x01) ? true : false;
	m_ta = ((_register[RDSB] >> 4) & 0x01) ? true : false;

	m_pty.UpdatePtyType(_register[RDSB] & 0x03, (_register[RDSB] >> 2) & 0x01);

	if(m_bC)
		UpdateAFList(_register);
	
	if(m_bD)
	{
		const BYTE addr = (_register[RDSB] & 0x03) * 2;
		m_ps.Update(addr+0, _register[RDSD] >> 8);
		m_ps.Update(addr+1, _register[RDSD] & 0xff);
	}
}

void CRadioRds::RDS_0B_Function( const WORD * _register )
{
	m_pty.UpdatePtyType(_register[RDSB] & 0x03, (_register[RDSB] >> 2) & 0x01);
	m_ta = ((_register[RDSB] >> 4) & 0x01) ? true : false;

	if(m_bD)
	{
		const BYTE addr = (_register[RDSB] & 0x03) * 2;
		m_ps.Update(addr+0, _register[RDSD] >> 8);
		m_ps.Update(addr+1, _register[RDSD] & 0xff);
	}
}


void CRadioRds::RDS_3A_Function( const WORD * _register )
{
	if(!m_bC || !m_bD) return;
	const WORD appID = _register[RDSD];
	switch (appID)
	{
		case 0xCD46:
		case 0x0D45: //TMC ODA
		{
			OutputDebugString(L"TMC ODA detected\n");
			const BYTE variant = ((_register[RDSC] >> 14) & 0x03);
			if(variant == 0)
			{
				m_TMC.LTN = ((_register[RDSC] >> 6) & 0x3F);
				m_TMC.AFI = ((_register[RDSC] >> 5) & 0x01);
				m_TMC.M = ((_register[RDSC] >> 4) & 0x01);
				m_TMC.I = ((_register[RDSC] >> 3) & 0x01);
				m_TMC.N = ((_register[RDSC] >> 2) & 0x01);
				m_TMC.R = ((_register[RDSC] >> 1) & 0x01);
				m_TMC.U = (_register[RDSC] & 0x01);
#ifdef _DEBUG
				std::wstringstream str;
				str << L"LTN " << m_TMC.LTN << ((!m_TMC.LTN )? L"(Encrypted)" : L"(Clear)") << L" - "
					<< L"AFI: " << m_TMC.AFI << L" - "
					<< L"M: " << m_TMC.M << L" - "
					<< L"I: " << m_TMC.I << L" - "
					<< L"N: " << m_TMC.N << L" - "
					<< L"R: " << m_TMC.R << L" - "
					<< L"U: " << m_TMC.U << std::endl;
				OutputDebugString(str.str().c_str());
#endif
			}
			else if(variant == 1)
			{
				m_TMC.G = ((_register[RDSC] >> 12) & 0x03);
				m_TMC.sID = ((_register[RDSC] >> 6) & 0x3F);
#ifdef _DEBUG
				std::wstringstream str;
				str << L"SID: " << m_TMC.sID << L" - "
					<< L"Gap: " << m_TMC.G;
#endif
				if(m_TMC.M)
				{
					m_TMC.Ta = ((_register[RDSC] >> 4) & 0x03);
					m_TMC.Tw = ((_register[RDSC] >> 2) & 0x03);
					m_TMC.Td = (_register[RDSC] & 0x03);
#ifdef _DEBUG
					str << L" - "
						<< L"Ta: " << m_TMC.Ta << L" - "
						<< L"Tw: " << m_TMC.Tw << L" - "
						<< L"Td: " << m_TMC.Td;
#endif
				}
#ifdef _DEBUG
				str << std::endl;
				OutputDebugString(str.str().c_str());
#endif
			}
		}
		break;
		case 0x4BD7: //RT PLUS ODA
		{
			m_rtPlusTagODA = ((_register[RDSB] >> 0x01) & 0xF);
#ifdef _DEBUG
			std::wstringstream str;
			str << L"____________RT PLUS 3A DETECTED__________" << std::endl
				<< L"check ODA " << m_rtPlusTagODA << L"A" << std::endl
				<< L"_________________________________________" << std::endl;
			OutputDebugString(str.str().c_str());
#endif
		}
		break;
	}
}

void CRadioRds::RDS_4A_Function( const WORD * _register )
{
	if(!m_bC || !m_bD) return;
	m_timeDate.Update(_register[RDSB],_register[RDSC],_register[RDSD]);
}


void CRadioRds::RDS_8A_Function( const WORD * _register )
{
	if(!m_bC || !m_bD) return;
	if(m_TMC.LTN == 0xFF) return;
	if((ERRORS_CORRECTED(m_errorFlags,BLOCK_C) != CORRECTED_NONE) ||
		(ERRORS_CORRECTED(m_errorFlags,BLOCK_D) != CORRECTED_NONE))
		return;
	if((_register[RDSB] & 0x1F) == 0) //encrypted
	{
		m_TMC.encrypted = (_register[RDSB] & 0x1F);
		m_TMC.encryptedVariantCode = ((_register[RDSC] >> 13) & 0x07); //should be 0
		m_TMC.encryptedTest = ((_register[RDSC] >> 11) & 0x03); //should be 3
		m_TMC.sID = ((_register[RDSC] >> 5) & 0x3F);
		m_TMC.encID = (_register[RDSC] & 0x1F);
		m_TMC.LTNBE = ((_register[RDSD] >> 10) & 0x3F);
#ifdef _DEBUG
		std::wstringstream str;
		str << L"Encrypted bytes: " << m_TMC.encrypted << L" - "
			<< L"Encrypted Variant Code: " << m_TMC.encryptedVariantCode <<  L" - "
			<< L"Encrypted test: " << m_TMC.encryptedTest << L" - "
			<< L"Encrypted SID: " << m_TMC.sID << L" - "
			<< L"Encryption security table: " << m_TMC.encID << L" - "
			<< L"LTN before encryption: " << m_TMC.LTNBE << std::endl;
		OutputDebugString(str.str().c_str());
#endif
	}
	else
	{
		m_TMC.T = ((_register[RDSB] >> 4) & 0x01);
		m_TMC.F = ((_register[RDSB] >> 3) & 0x01);
		if(m_TMC.T == 0) // user message
		{
			if(m_TMC.F == 1) //single group
			{
				m_TMC.DP_CI = (_register[RDSB] & 0x07);
				m_TMC.D = ((_register[RDSC] >> 15) & 0x01);
				m_TMC.Y14_SG = ((_register[RDSC] >> 14) & 0x01);
				m_TMC._extent = ((_register[RDSC] >> 11) & 0x07);
				m_TMC._event = (_register[RDSC] & 0x07FF);
				m_TMC.location = _register[RDSD];

				m_bits.Reset();
				// struct completed notify window
				::PostMessage(m_hWnd,m_userMessage,1,0);
#ifdef _DEBUG
				std::wstringstream str;
				str << L"__________SINGLE GROUP EVENT___________" << std::endl
					<< L"DP: " << m_TMC.DP_CI << L" - "
					<< L"D: " << m_TMC.D << L" - "
					<< L"Y14: " << m_TMC.Y14_SG << L" - "
					<< L"Extent: " << m_TMC._extent << L" - "
					<< L"Event: " << m_TMC._event << L" - "
					<< L"Location: " << m_TMC.location << std::endl;
				OutputDebugString(str.str().c_str());
#endif
			}
			else // multi group
			{
				m_TMC.DP_CI = (_register[RDSB] & 0x07);
				m_TMC.D = ((_register[RDSC] >> 15) & 0x01);
				if(m_TMC.D == 1) // first message in group
				{
					m_TMC.Y14_SG = ((_register[RDSC] >> 14) & 0x01);
					m_TMC._extent = ((_register[RDSC] >> 11) & 0x07);
					m_TMC._event = (_register[RDSC] & 0x07FF);
					m_TMC.location = _register[RDSD];

#ifdef _DEBUG
					std::wstringstream str;
					str << L"__________MULTI GROUP EVENT___________" << std::endl
						<< L"CI: " << m_TMC.DP_CI << L" - "
						<< L"D: " << m_TMC.D << L" - "
						<< L"Y14: " << m_TMC.Y14_SG << L" - "
						<< L"Extent: " << m_TMC._extent << L" - "
						<< L"Event: " << m_TMC._event << L" - "
						<< L"Location: " << m_TMC.location << std::endl;
					OutputDebugString(str.str().c_str());
#endif
				}
				else // subsequent messages
				{
					m_TMC.Y14_SG = ((_register[RDSC] >> 14) & 0x01);
					BYTE lastGSI = m_TMC.GSI;
					m_TMC.GSI = ((_register[RDSC] >> 12) & 0x03);
					if(m_TMC.Y14_SG)
					{
						m_bits.Reset();
						if(lastGSI != m_TMC.GSI)
							lastGSI = m_TMC.GSI + 1;
					}
					if(m_TMC.GSI == lastGSI - 1)
					{
						m_bits.AddBits(_register[RDSC] & 0xFFF,12);
						while(ConsumeTMCBits(m_TMC.lastLabel < 16));
						m_bits.AddBits(_register[RDSD],16);
						while(ConsumeTMCBits(m_TMC.lastLabel < 16));
					}
					else //to do 
					{
						// struct completed notify window
						// ::PostMessage(m_hWnd,m_userMessage,1,0);
					}
				}
			}
		}
		else
		{
			//Tuning information
		}
	}
}

void CRadioRds::CallRDSFunction( const RDS_GROUP_TYPE & groupType, const WORD * _register )
{
	m_bA = (ERRORS_CORRECTED(m_errorFlags,BLOCK_A) <= m_rdsBlerMax[0]);
	m_bB = (ERRORS_CORRECTED(m_errorFlags,BLOCK_B) <= m_rdsBlerMax[1]);
	m_bC = (ERRORS_CORRECTED(m_errorFlags,BLOCK_C) <= m_rdsBlerMax[2]);
	m_bD = (ERRORS_CORRECTED(m_errorFlags,BLOCK_D) <= m_rdsBlerMax[3]);

	m_pty.Validate((_register[RDSB] >> 5) & 0x1F);
	ValidateTP(_register[RDSB]);

	if (m_bA)
		ValidatePI(_register[RDSA]);
	if((groupType & 0x01) && m_bC)
		ValidatePI(_register[RDSC]);

	auto it = m_RDSFunctionsMap.find(groupType);
	if(it != m_RDSFunctionsMap.cend())
	{
		RDSFUNC f = it->second;
		(this->*f)(_register);
	}
}

void CRadioRds::UpdateRDS( const WORD * _register )
{
	if(GetTickCount() - m_startValidTime < 1000) return;
	auto errorCount = 0;
	m_errorFlags = 0;
	auto rdsv = (_register[POWERCFG] & POWERCFG_RDSM);

	if(rdsv)
	{
		m_blerRegisters[0] = (_register[STATUSRSSI] & STATUSRSSI_BLERA) >> 9;
		m_blerRegisters[1] = (_register[READCHAN] & READCHAN_BLERB) >> 14;
		m_blerRegisters[2] = (_register[READCHAN] & READCHAN_BLERC) >> 12;
		m_blerRegisters[3] = (_register[READCHAN] & READCHAN_BLERD) >> 10;

		for (auto field = 0; field < 4; field++)
		{
			if (m_blerRegisters[field] == UNCORRECTABLE)
			{
				errorCount++;
			}
			m_errorFlags = m_errorFlags << 2 | m_blerRegisters[field];
		}
	}
	else
	{
		errorCount = (_register[STATUSRSSI] & 0x0E00) >> 9;
		m_errorFlags = 0;
	}
	
	if (errorCount && !rdsv)
	{
		return;
	}

	if (ERRORS_CORRECTED(m_errorFlags, BLOCK_B) <= m_rdsBlerMax[1])
	{
		const RDS_GROUP_TYPE groupType = static_cast<RDS_GROUP_TYPE>(_register[RDSB] >> 11);
		CallRDSFunction(groupType,_register);
	}
}

void CRadioRds::InitRDS()
{
	memset(m_blerRegisters,0,sizeof(BYTE) * 4);
	m_errorFlags = 0;

	m_pty.Reset();
	m_ps.Reset();
	m_af.Reset();
	m_ptyn.Reset();
	m_radioText.Reset();
	m_timeDate.Reset();

	m_piLast = 0;
	m_piValidateCounter = 1;
	m_piRegionString.clear();
	m_piCountryString.clear();
	m_ecc = 0;

	m_languageSpoken.clear();

	m_ms = 0;

	m_ta = 0;
	m_tp = 0;
	
	m_trafficNowPlaying = false;
	m_taValidateCounter = 1;

	memset(&m_TMC,0,sizeof(SKYRADIO_TMCStruct));
	m_TMC.LTN = 0xFF;
	m_TMC.lastLabel = 16;
		

	m_piDay = 0;
	m_piHour = 0;
	m_piMinute = 0;

	m_rtPlusTagODA = 16;
	memset(&m_rtplusContent[0],0,sizeof(SKYRADIO_RTContent));
	memset(&m_rtplusContent[1],0,sizeof(SKYRADIO_RTContent));

	m_tmpEonList.clear();
	m_EonList.clear();
	m_startValidTime = GetTickCount();
}

void CRadioRds::ValidateTP( const WORD currentRegister )
{
	m_tp = ((currentRegister >> 10) & 0x01);

	if(m_tp && m_ta)
	{
		if(m_taValidateCounter > TA_VALIDATE_LIMIT)
		{
			m_taValidateCounter = TA_VALIDATE_LIMIT + 1;
			if(!m_trafficNowPlaying)
			{
				m_trafficNowPlaying = true;
				::PostMessage(m_hWnd,m_userMessage,0,1);
			}
		}
		else
			m_taValidateCounter++;
	}
	else
	{
		m_taValidateCounter = 1;
		if(m_trafficNowPlaying)
		{
			m_trafficNowPlaying = false;
			::PostMessage(m_hWnd,m_userMessage,0,0);
		}
	}
}

const bool CRadioRds::GetTAFlag()
{
	return (m_ta == 1);
}

const bool CRadioRds::GetTPFlag()
{
	return (m_tp == 1);
}

const bool CRadioRds::GetMSFlag()
{
	return (m_ms == 1);
}

void CRadioRds::UpdateAFList( const WORD * _register )
{
	m_af.Validate((_register[RDSC] >> 8) & 0xFF, true);
	m_af.Validate(_register[RDSC] & 0xFF, true);
}

const std::set<unsigned int> & CRadioRds::GetAFList() const
{
	return m_af.GetAFList();
}

void CRadioRds::RDS_10A_Function( const WORD * _register )
{
	const bool C0 = (_register[RDSB] & 0x01);
	if(!m_bC || !m_bD) return;

	if(!C0)
	{
		m_ptyn.Update(0,(_register[RDSC] >> 8) & 0xFF);
		m_ptyn.Update(1,_register[RDSC] & 0xFF);
		m_ptyn.Update(2,(_register[RDSD] >> 8) & 0xFF);
		m_ptyn.Update(3,_register[RDSD] & 0xFF);
	}
	else
	{
		m_ptyn.Update(4,(_register[RDSC] >> 8) & 0xFF);
		m_ptyn.Update(5,_register[RDSC] & 0xFF);
		m_ptyn.Update(6,(_register[RDSD] >> 8) & 0xFF);
		m_ptyn.Update(7,_register[RDSD] & 0xFF);
	}
}

void CRadioRds::RDS_12A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 12)
		DecodeRTPlusTag(_register);
}

void CRadioRds::RDS_14A_Function( const WORD * _register )
{
	if(ERRORS_CORRECTED(m_errorFlags,BLOCK_D) == CORRECTED_NONE)
	{
		const BYTE eonVariant = (_register[RDSB] & 0xF);

		// Linkage Information PI(ON) same as PI(TN)
		// see 3.2.1.8.3
		if(eonVariant != 12) 
		{
			if(_register[RDSD])
			{
				auto it = m_EonList.find(_register[RDSD]);
				if(it == m_EonList.end())
				{
					it = m_tmpEonList.find(_register[RDSD]);
					if(it == m_tmpEonList.end())
					{
						CEon thisEonStruct;
						thisEonStruct.m_ta = 0;
						thisEonStruct.m_tp = 0;
						thisEonStruct.m_validateCount = 1;
						auto insertion = m_tmpEonList.insert(std::make_pair(_register[RDSD],thisEonStruct));
						it = insertion.first;
					}
					else
					{
						it->second.m_validateCount++;
						if(it->second.m_validateCount > 2)
						{
							auto insertion = m_EonList.insert(*it);
							m_tmpEonList.erase(it);
							it = insertion.first;
						}
					}	
				}

				it->second.m_tp = ((_register[RDSB] >> 4) & 0x01);
				switch(eonVariant)
				{
				case 0:
				case 1:
				case 2:
				case 3:
					{
						it->second.m_ps.Update((2 * eonVariant) + 0,(_register[RDSC] >> 8) & 0xFF);
						it->second.m_ps.Update((2 * eonVariant) + 1,(_register[RDSC] & 0xFF));
					}
					break;
				case 4:
					{
						it->second.m_af.Validate(((_register[RDSC] >> 8) & 0xFF),true);
						it->second.m_af.Validate((_register[RDSC] & 0xFF),false);
					}
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					{
						m_af.Validate(((_register[RDSC] >> 8) & 0xFF),true);
						it->second.m_af.Validate((_register[RDSC] & 0xFF),false);
					}
					break;
				case 9: // Discard EON AM frequencies
					{
						m_af.Validate(((_register[RDSC] >> 8) & 0xFF),true);
					}
					break;
				case 13:
					{
						it->second.m_pty.Validate((_register[RDSC] >> 11) & 0x1F);
						it->second.m_ta = (_register[RDSC] & 0x01);
					}
					break;
				}
			}
		}
	}
}


void CRadioRds::RDS_14B_Function( const WORD * _register )
{
	if(ERRORS_CORRECTED(m_errorFlags,BLOCK_D) == CORRECTED_NONE && _register[RDSD])
	{
		auto it = m_EonList.find(_register[RDSD]);
		if(it == m_EonList.end())
		{
			it = m_tmpEonList.find(_register[RDSD]);
			if(it == m_tmpEonList.end())
			{
				CEon thisEonStruct;
				thisEonStruct.m_ta = 0;
				thisEonStruct.m_tp = 0;
				thisEonStruct.m_validateCount = 1;
				auto insertion = m_tmpEonList.insert(std::make_pair(_register[RDSD],thisEonStruct));
				it = insertion.first;
			}
			else
			{
				it->second.m_validateCount++;
				if(it->second.m_validateCount > 2)
				{
					auto insertion = m_EonList.insert(*it);
					m_tmpEonList.erase(it);
					it = insertion.first;
				}
			}	
		}

		it->second.m_tp = ((_register[RDSB] >> 4) & 0x01);
		it->second.m_ta = ((_register[RDSB] >> 3) & 0x01);
	}
}


void CRadioRds::RDS_15B_Function( const WORD * _register )
{
	m_ms = ((_register[RDSB] >> 3) & 0x01) ? true : false;
	m_ta = ((_register[RDSB] >> 4) & 0x01) ? true : false;

	m_pty.UpdatePtyType(_register[RDSB] & 0x03, (_register[RDSB] >> 2) & 0x01);
}

const std::wstring & CRadioRds::GetPtyNDisplayString() const
{
	return m_ptyn.GetDisplayString();
}

void CRadioRds::ValidatePI( const WORD currentPi )
{
	if(currentPi != m_piLast)
	{
		m_piLast = currentPi;
		m_piValidateCounter = 1;
	}
	else
	{
		m_piValidateCounter++;
	}
	if(m_piValidateCounter > PI_VALIDATE_LIMIT)
	{
		m_piValidateCounter = PI_VALIDATE_LIMIT + 1;
		UpdatePI(m_piLast);
	}
}

void CRadioRds::UpdatePI( const WORD currentPi )
{
	m_piRegionString = m_countryMap.GetPiRegionDisplayString((currentPi >> 8) & 0x0F);
	m_piCountryString = m_countryMap.GetCountry((currentPi >> 12) & 0x0F, m_ecc);
}

const std::wstring & CRadioRds::GetRegionString() const
{
	return m_piRegionString;
}

const std::wstring & CRadioRds::GetCountryString() const
{
	return m_piCountryString;
}

const unsigned int CRadioRds::GetCurrentPI() const
{
	if(m_piValidateCounter > PI_VALIDATE_LIMIT)
		return m_piLast;
	return 0;
}

void CRadioRds::RDS_1A_Function( const WORD * _register )
{
	m_tp = ((_register[RDSB] >> 10) & 0x01);

	if(m_bC)
	{
		const BYTE variant = (_register[RDSC] >> 12) & 0x07;
		switch(variant)
		{
			case 0:
			{
				m_ecc = (_register[RDSC] & 0xFF);
				if(m_bD)
					ValidateProgramItem(_register[RDSD]);
			}
			break;
			case 2:
			{
				if(m_bD)
					ValidateProgramItem(_register[RDSD]);
			}
			break;
			case 3:
			{
				m_languageSpoken = m_countryMap.GetLanguage(_register[RDSC] & 0xFF);
			}
			break;
		}
	}
}

void CRadioRds::RDS_1B_Function( const WORD * _register )
{
	m_tp = ((_register[RDSB] >> 10) & 0x01);
	if(m_bD)
		ValidateProgramItem(_register[RDSD]);
}


void CRadioRds::RDS_2A_Function( const WORD * _register )
{
	m_radioText.Update((_register[RDSB] >> 4) & 0x01,4,(_register[RDSB] & 0xF) * 4,m_errorFlags,(BYTE*)&(_register[RDSC]));
}

void CRadioRds::RDS_2B_Function( const WORD * _register )
{
	m_radioText.Update((_register[RDSB] >> 4) & 0x01,2,(_register[RDSB] & 0xF) * 2,m_errorFlags,(BYTE*)&(_register[RDSD]));
}

const std::wstring & CRadioRds::GetLanguageString() const
{
	return m_languageSpoken;
}

void CRadioRds::ValidateProgramItem( const WORD currentItem )
{
	m_piDay = (currentItem >> 11) & 0x1F;
	if(m_piDay)
	{
		m_piHour = (currentItem >> 6) & 0x1F;
		m_piMinute = (currentItem & 0x3F);
	}
	else
	{
		const bool subType = ((currentItem >> 10) & 0x01);
		if(subType)
		{
			BYTE subtype1 = ((currentItem >> 8) & 0x03);
			if(!subtype1)
			{
				m_ecc = (currentItem & 0xFF);
			}
		}
	}
}

const CTimeDate & CRadioRds::GetCurrentRadioTime() const
{
	return m_timeDate;
}

const BYTE * CRadioRds::GetPtyType()
{
	return m_pty.GetPtyType();
}

const std::wstring & CRadioRds::GetRadioTextString() const
{
	return m_radioText.GetRadioTextString();
}

const std::wstring & CRadioRds::GetPSDisplayString() const
{
	return m_ps.GetDisplayString();
}

const std::map<unsigned int, CEon> & CRadioRds::GetEONList() const
{
	return m_EonList;
}

const SKYRADIO_TMCStruct & CRadioRds::GetTMCData() const
{
	return m_TMC;
}

bool CRadioRds::ConsumeTMCBits( bool partial )
{
	if(!partial)
	{
		WORD label = 0;
		if(m_bits.GetBits(label,4))
		{
			m_TMC.label = static_cast<BYTE>(label);
			auto it = m_ldmMap.find(m_TMC.label);
			if(it != m_ldmMap.end())
			{
				WORD labelData = 0;
				if(m_bits.GetBits(labelData,it->second.bitsInvolved))
				{
					m_TMC.labelData = labelData;
					if(labelData == 0 && m_TMC.label == 0)
					{
						m_TMC.lastLabel = 16;
						return false;
					}

#ifdef _DEBUG
					std::wstringstream str;
					str << L"Second Group indicator: " << m_TMC.Y14_SG << L" - "
						<< L"Group Sequence indicator: " << m_TMC.GSI << L" - "
						<< L"Label: " << m_TMC.label << L" - "
						<< L"Label Data: " << m_TMC.labelData << L" - "
						<< L"Label description: " << it->second.description << std::endl;
					OutputDebugString(str.str().c_str());
#endif
					m_TMC.lastLabel = 16;
					return true;
				}
				else
				{
					m_TMC.lastLabel = m_TMC.label;
					return false;
				}
			}
		}
		else return false;
	}
	else
	{
		auto it = m_ldmMap.find(m_TMC.label);
		if(it != m_ldmMap.end())
		{
			WORD labelData = 0;
			if(m_bits.GetBits(labelData,it->second.bitsInvolved))
			{
				m_TMC.labelData = labelData;
				if(labelData == 0 && m_TMC.label == 0)
				{
					m_bits.Reset();
					return false;
				}
				
#ifdef _DEBUG
				std::wstringstream str;
				str << L"Second Group indicator: " << m_TMC.Y14_SG << L" - "
					<< L"Group Sequence indicator: " << m_TMC.GSI << L" - "
					<< L"Label: " << m_TMC.label << L" - "
					<< L"Label Data: " << m_TMC.labelData << L" - "
					<< L"Label description: " << it->second.description << std::endl;
				OutputDebugString(str.str().c_str());
#endif
				m_TMC.lastLabel = 16;
				return true;
			}
			else
			{
				m_TMC.lastLabel = m_TMC.label;
				return false;
			}
		}
	}
	return false;
}

void CRadioRds::DecodeRTPlusTag( const WORD * _register )
{
	//const bool tooglebit = (((_register[RDSB] >> 4) & 0x01) == 0x01);
	m_rtplusContent[0].contentType = ((_register[RDSB] & 0x7) << 3) | ((_register[RDSC] >> 13) & 0x3);
	m_rtplusContent[0].startMarker = ((_register[RDSC] >> 7) & 0x3F);
	m_rtplusContent[0].lengthMarker = ((_register[RDSC] >> 1) & 0x3F);

#ifdef _DEBUG
	std::wstringstream str;
	str << L"Content Type: " << m_rtplusContent[0].contentType << L" - "
		<< L"Start Marker: " << m_rtplusContent[0].startMarker << L" - "
		<< L"Length Marker: " << m_rtplusContent[0].lengthMarker << std::endl;
	OutputDebugString(str.str().c_str());
#endif

	m_rtplusContent[1].contentType = ((_register[RDSC] & 0x1) << 5) | ((_register[RDSD] >> 11) & 0x1F);
	m_rtplusContent[1].startMarker = ((_register[RDSD] >> 5) & 0x3F);
	m_rtplusContent[1].lengthMarker = (_register[RDSD] & 0x1F);

#ifdef _DEBUG
	str.str(L"");
	str << L"Content Type: " << m_rtplusContent[1].contentType << L" - "
		<< L"Start Marker: " << m_rtplusContent[1].startMarker << L" - "
		<< L"Length Marker: " << m_rtplusContent[1].lengthMarker << std::endl;
	OutputDebugString(str.str().c_str());
#endif
}

void CRadioRds::RDS_5A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 5)
		DecodeRTPlusTag(_register);
}

void CRadioRds::RDS_6A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 6)
		DecodeRTPlusTag(_register);
}

void CRadioRds::RDS_7A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 7)
		DecodeRTPlusTag(_register);
}

void CRadioRds::RDS_9A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 9)
		DecodeRTPlusTag(_register);
}

void CRadioRds::RDS_11A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 11)
		DecodeRTPlusTag(_register);
}

void CRadioRds::RDS_13A_Function( const WORD * _register )
{
	if(m_rtPlusTagODA == 13)
		DecodeRTPlusTag(_register);
}

const SKYRADIO_RTContent * CRadioRds::GetRTPlusContent() const
{
	return &m_rtplusContent[0];
}
