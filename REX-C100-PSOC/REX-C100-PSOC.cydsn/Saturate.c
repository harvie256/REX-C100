#include "Saturate.h"

int32 sadd32(int32 a, int32 b)
{
    // Test for different signs
	int aSign =  (a & (1<<31));
	int bSign = (b & (1<<31));

	if(aSign ^ bSign)
    {
        // If signs are different, it can not overflow.
        return a + b;
    } else {
        int32 result = a + b;
		if(aSign ^ (result & (1<<31)))
        {
            // Signs are different, so overflow occurred
            if(a & (1<<31))
            {
                return (1<<31); // int32 min value
            } else {
                return ~(1<<31); // int32 max value
            }
        } else {
            // Overflow did not occour
			return result;
        }
    }
}

int16 sadd16(int16 a, int16 b)
{
    // Test for different signs
	int aSign =  (a & (1<<15));
	int bSign = (b & (1<<15));

	if(aSign ^ bSign)
    {
        // If signs are different, it can not overflow.
        return a + b;
    } else {
        int16 result = a + b;
		if(aSign ^ (result & (1<<15)))
        {
            // Signs are different, so overflow occurred
            if(a & (1<<15))
            {
                return -32768; // int16 min value
            } else {
                return 32767; // int16 max value
            }
        } else {
            // Overflow did not occour
			return result;
        }
    }
}

uint16 saddu16(uint16 a, uint16 b)
    { return (a > 0xFFFF - b) ? 0xFFFF : a + b; }

uint32 saddu32(uint32 a, uint32 b)
    { return (a > 0xFFFFFFFF - b) ? 0xFFFFFFFF : a + b;}

