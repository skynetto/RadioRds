#include "StdAfx.h"
#include "BitsManipulator.h"
#include<algorithm>

CBitsManipulator::CBitsManipulator(void)
{
}


CBitsManipulator::~CBitsManipulator(void)
{
}

void CBitsManipulator::AddBits( WORD val, short numBits )
{
	bits.reserve(bits.size() + numBits);
	val <<= (16 - numBits);
	while(numBits > 0)
	{
		unsigned short bit = (val >> 15);
		bits.push_back(bit);
		val <<= 1;
		numBits--;
	}
}

void CBitsManipulator::Reset()
{
	bits.clear();
}

bool CBitsManipulator::GetBits( WORD & val, short numBits )
{
	if(static_cast<short>(bits.size()) < numBits)
		return false;
	for(short i = 0; i < numBits; i++)
	{
		val |= bits[i] << (numBits - i - 1);
	}
	if(numBits != 0)
		bits.erase(bits.begin(), bits.begin()+numBits);
	return true;
}
