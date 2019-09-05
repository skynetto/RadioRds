#include "StdAfx.h"
#include "Af.h"


CAf::CAf(void)
{
	Reset();
}


CAf::~CAf(void)
{
}

void CAf::Reset()
{
	m_vhfType = false;
	m_afList.clear();
	m_afSize = 0;
}

const std::set<unsigned int> & CAf::GetAFList() const
{
	return m_afList;
}

void CAf::Validate( const BYTE af_code, bool checkVhfType )
{
	if(af_code >= 224 && af_code <= 249)
	{
		m_afSize = af_code - 224;
		m_vhfType = true;
	}
	if(checkVhfType)
	{
		if(m_vhfType && (m_afSize > static_cast<int>(m_afList.size()))) //only FM AF permitted
		{
			if((af_code > 0) && (af_code <= 204))
			{
				m_afList.insert((af_code + 875) * 10);
			}
		}
	}
	else
	{
		if((af_code > 0) && (af_code <= 204))
		{
			m_afList.insert((af_code + 875) * 10);
			m_afSize = m_afList.size();
		}
	}
}
