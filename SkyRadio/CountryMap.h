#pragma once

#include <map>
#include <string>

struct  eccToCountry
{
public:
	explicit eccToCountry(const unsigned short country)
	{
		InitializeEccToCountry(country);
	};

	const std::wstring GetCountry(const unsigned short ecc);
private:
	void InitializeEccToCountry(const unsigned short country);
	std::map<unsigned short,std::wstring> m_eccToCountryMap;
};


class CCountryMap
{
public:
	CCountryMap(void);
	virtual ~CCountryMap(void);

	const std::wstring GetCountry(const unsigned short country, const unsigned short ecc);
	const std::wstring GetLanguage(const unsigned short languageCode);
	const std::wstring GetPiRegionDisplayString(const BYTE & region);
private:

	std::map<unsigned short, eccToCountry> m_eccValidMap;
	std::map<unsigned short, std::wstring> m_eccInvalidMap;
	std::map<unsigned short, std::wstring> m_languageMap;
	std::map<unsigned short, std::wstring> m_piRegionMap;

	void InitializeEccInvalidMap(void);
	void InitializePiRegion(void);
	void InitializeLanguageMap(void);
};

