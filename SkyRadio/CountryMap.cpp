#include "StdAfx.h"
#include "CountryMap.h"


CCountryMap::CCountryMap(void)
{
	InitializeEccInvalidMap();
	InitializeLanguageMap();
	InitializePiRegion();
}


CCountryMap::~CCountryMap(void)
{
}

void CCountryMap::InitializeEccInvalidMap()
{
	m_eccInvalidMap[0x01] = L"Germany/Greece/Moldova/Morocco";
	m_eccInvalidMap[0x02] = L"Algeria/Cyprus/Czech Republic/Estonia/Ireland";
	m_eccInvalidMap[0x03] = L"Andorra/Poland/San Marino/Turkey";
	m_eccInvalidMap[0x04] = L"Israel/Macedonia/Switzerland/Vatican";
	m_eccInvalidMap[0x05] = L"Italy/Jordan/Slovakia";
	m_eccInvalidMap[0x06] = L"Belgium/Finland/Syria/Ukraine";
	m_eccInvalidMap[0x07] = L"Luxembourg/Russian Federation/Tunisia";
	m_eccInvalidMap[0x08] = L"Azores/Bulgaria/Madeira/Netherlands/Palestine/Portugal";
	m_eccInvalidMap[0x09] = L"Albania/Denmark/Faeroe/Latvia/Lichtenstein/Slovenia";
	m_eccInvalidMap[0x0A] = L"Austria/Gibraltars/Iceland/Lebanon";
	m_eccInvalidMap[0x0B] = L"Hungary/Iraq/Monaco";
	m_eccInvalidMap[0x0C] = L"Croatia/Lithuania/Malta/United Kingdom";
	m_eccInvalidMap[0x0D] = L"Germany/Libya/Yugoslavia";
	m_eccInvalidMap[0x0E] = L"Canaries/Romania/Spain/Sweden";
	m_eccInvalidMap[0x0F] = L"Belarus/Bosnia Herzegovina/Egypt/France/Norway";

	for(unsigned short i = 0x01; i < 0x10; i++)
		m_eccValidMap.insert(std::make_pair(i,eccToCountry(i)));
}

const std::wstring CCountryMap::GetCountry( const unsigned short country, const unsigned short ecc )
{
	if(ecc == 0)
	{
		auto it = m_eccInvalidMap.find(country);
		if(it != m_eccInvalidMap.cend())
			return it->second;
	}
	else
	{
		auto it = m_eccValidMap.find(country);
		if(it != m_eccValidMap.cend())
			return it->second.GetCountry(ecc);
	}
	return L"Unknown";
}

void CCountryMap::InitializeLanguageMap()
{
	m_languageMap[0x00] = L"Unknown";
	m_languageMap[0x01] = L"Albanian";
	m_languageMap[0x02] = L"Breton";
	m_languageMap[0x03] = L"Catalan";
	m_languageMap[0x04] = L"Croatian";
	m_languageMap[0x05] = L"Welsh";
	m_languageMap[0x06] = L"Czech";
	m_languageMap[0x07] = L"Danish";
	m_languageMap[0x08] = L"German";
	m_languageMap[0x09] = L"English";
	m_languageMap[0x0A] = L"Spanish";
	m_languageMap[0x0B] = L"Esperanto";
	m_languageMap[0x0C] = L"Estonian";
	m_languageMap[0x0D] = L"Basque";
	m_languageMap[0x0E] = L"Faroese";
	m_languageMap[0x0F] = L"French";
	m_languageMap[0x10] = L"Frisian";
	m_languageMap[0x11] = L"Irish";
	m_languageMap[0x12] = L"Gaelic";
	m_languageMap[0x13] = L"Galician";
	m_languageMap[0x14] = L"Icelandic";
	m_languageMap[0x15] = L"Italian";
	m_languageMap[0x16] = L"Lappish";
	m_languageMap[0x17] = L"Latin";
	m_languageMap[0x18] = L"Latvian";
	m_languageMap[0x19] = L"Luxembourgian";
	m_languageMap[0x1A] = L"Lithuanian";
	m_languageMap[0x1B] = L"Hungarian";
	m_languageMap[0x1C] = L"Maltese";
	m_languageMap[0x1D] = L"Dutch";
	m_languageMap[0x1E] = L"Norwegian";
	m_languageMap[0x1F] = L"Occitan";
	m_languageMap[0x20] = L"Polish";
	m_languageMap[0x21] = L"Portuguese";
	m_languageMap[0x22] = L"Romanian";
	m_languageMap[0x23] = L"Romansh";
	m_languageMap[0x24] = L"Serbian";
	m_languageMap[0x25] = L"Slovak";
	m_languageMap[0x26] = L"Slovene";
	m_languageMap[0x27] = L"Finnish";
	m_languageMap[0x28] = L"Swedish";
	m_languageMap[0x29] = L"Turkish";
	m_languageMap[0x2A] = L"Flemish";
	m_languageMap[0x2B] = L"Walloon";
}

const std::wstring CCountryMap::GetLanguage( const unsigned short languageCode )
{
	auto it = m_languageMap.find(languageCode);
	if(it != m_languageMap.cend())
		return it->second;
	return m_languageMap[0];
}

void CCountryMap::InitializePiRegion()
{
	m_piRegionMap[0x00] = L"Local";
	m_piRegionMap[0x01] = L"International";
	m_piRegionMap[0x02] = L"National";
	m_piRegionMap[0x03] = L"Supra-Regional";
	m_piRegionMap[0x04] = L"Region 1";
	m_piRegionMap[0x05] = L"Region 2";
	m_piRegionMap[0x06] = L"Region 3";
	m_piRegionMap[0x07] = L"Region 4";
	m_piRegionMap[0x08] = L"Region 5";
	m_piRegionMap[0x09] = L"Region 6";
	m_piRegionMap[0x0A] = L"Region 7";
	m_piRegionMap[0x0B] = L"Region 8";
	m_piRegionMap[0x0C] = L"Region 9";
	m_piRegionMap[0x0D] = L"Region 10";
	m_piRegionMap[0x0E] = L"Region 11";
	m_piRegionMap[0x0F] = L"Region 12";
}


const std::wstring CCountryMap::GetPiRegionDisplayString( const BYTE & region )
{
	auto it = m_piRegionMap.find(region);
	if(it != m_piRegionMap.cend())
		return it->second;
	return std::wstring();
}
void eccToCountry::InitializeEccToCountry( const unsigned short country )
{
	switch(country)
	{
		case 0x01: 
		{
			m_eccToCountryMap[0xE0] = L"Germany";
			m_eccToCountryMap[0xE1] = L"Greece";
			m_eccToCountryMap[0xE2] = L"Morocco";
			m_eccToCountryMap[0xE4] = L"Moldova";
		}
		break;
		case 0x02: 
		{
			m_eccToCountryMap[0xE0] = L"Algeria";
			m_eccToCountryMap[0xE1] = L"Cyprus";
			m_eccToCountryMap[0xE2] = L"Czech Republic";
			m_eccToCountryMap[0xE3] = L"Ireland";
			m_eccToCountryMap[0xE4] = L"Estonia";
		}
		break;
		case 0x03: 
		{
			m_eccToCountryMap[0xE0] = L"Andorra";
			m_eccToCountryMap[0xE1] = L"San Marino";
			m_eccToCountryMap[0xE2] = L"Poland";
			m_eccToCountryMap[0xE3] = L"Turkey";
		}
		break;
		case 0x04: 
		{
			m_eccToCountryMap[0xE0] = L"Israel";
			m_eccToCountryMap[0xE1] = L"Switzerland";
			m_eccToCountryMap[0xE2] = L"Vatican City";
			m_eccToCountryMap[0xE3] = L"Macedonia";
		}
		break;
		case 0x05: 
		{
			m_eccToCountryMap[0xE0] = L"Italy";
			m_eccToCountryMap[0xE1] = L"Jordan";
			m_eccToCountryMap[0xE2] = L"Slovakia";
		}
		break;
		case 0x06: 
		{
			m_eccToCountryMap[0xE0] = L"Belgium";
			m_eccToCountryMap[0xE1] = L"Finland";
			m_eccToCountryMap[0xE2] = L"Syria";
			m_eccToCountryMap[0xE4] = L"Ukraine";
		}
		break;
		case 0x07: 
		{
			m_eccToCountryMap[0xE0] = L"Russia";
			m_eccToCountryMap[0xE1] = L"Luxembourg";
			m_eccToCountryMap[0xE2] = L"Tunisia";
		}
		break;
		case 0x08: 
		{
			m_eccToCountryMap[0xE0] = L"Palestine";
			m_eccToCountryMap[0xE1] = L"Bulgaria";
			m_eccToCountryMap[0xE3] = L"Netherlands";
			m_eccToCountryMap[0xE4] = L"Portugal";
		}
		break;
		case 0x09:
		{
			m_eccToCountryMap[0xE0] = L"Albania";
			m_eccToCountryMap[0xE1] = L"Denmark";
			m_eccToCountryMap[0xE2] = L"Liechtenstein";
			m_eccToCountryMap[0xE3] = L"Latvia";
			m_eccToCountryMap[0xE4] = L"Slovenia";
		}
		break;
		case 0x0A:
		{
			m_eccToCountryMap[0xE0] = L"Austria";
			m_eccToCountryMap[0xE1] = L"Gibraltar";
			m_eccToCountryMap[0xE2] = L"Iceland";
			m_eccToCountryMap[0xE3] = L"Lebanon";
		}
		break;
		case 0x0B: 
		{
			m_eccToCountryMap[0xE0] = L"Hungary";
			m_eccToCountryMap[0xE1] = L"Iraq";
			m_eccToCountryMap[0xE2] = L"Monaco";
		}
		break;
		case 0x0C:
		{
			m_eccToCountryMap[0xE0] = L"Malta";
			m_eccToCountryMap[0xE1] = L"United Kingdom";
			m_eccToCountryMap[0xE2] = L"Lithuania";
			m_eccToCountryMap[0xE3] = L"Croatia";
		}
		break;
		case 0x0D:
		{
				m_eccToCountryMap[0xE0] = L"Germany";
				m_eccToCountryMap[0xE1] = L"Libya";
				m_eccToCountryMap[0xE2] = L"Yugoslavia";
		}
		break;
		case 0x0E:
		{
			m_eccToCountryMap[0xE1] = L"Romania";
			m_eccToCountryMap[0xE2] = L"Spain";
			m_eccToCountryMap[0xE3] = L"Sweden";
		}
		break;
		case 0x0F:
		{
			m_eccToCountryMap[0xE0] = L"Egypt";
			m_eccToCountryMap[0xE1] = L"France";
			m_eccToCountryMap[0xE2] = L"Norway";
			m_eccToCountryMap[0xE3] = L"Belarus";
			m_eccToCountryMap[0xE4] = L"Bosnia Herzegovina";
		}
		break;
	}
}

const std::wstring eccToCountry::GetCountry( const unsigned short ecc )
{
	auto it = m_eccToCountryMap.find(ecc);
	if(it != m_eccToCountryMap.cend())
		return it->second;
	return L"Unknown";
}
