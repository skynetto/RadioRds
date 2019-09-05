#ifndef Af_h__
#define Af_h__

#include <set>

class CAf
{
public:
	CAf(void);
	virtual ~CAf(void);

	void									Reset(void);
	const std::set<unsigned int> &			GetAFList() const;
	void									Validate( const BYTE af_code, bool checkVhfType);
private:
	std::set<unsigned int>					m_afList;
	bool									m_vhfType;
	size_t									m_afSize;
};

#endif // Af_h__
