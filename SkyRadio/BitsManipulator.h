#ifndef BitsManipulator_h__
#define BitsManipulator_h__

#include <vector>

class CBitsManipulator
{
public:
	CBitsManipulator(void);
	virtual ~CBitsManipulator(void);

	void AddBits(WORD val, short numBits);
	bool GetBits(WORD & val, short numBits);
	void Reset(void);
private:
	std::vector<unsigned short> bits;
};

#endif // BitsManipulator_h__
