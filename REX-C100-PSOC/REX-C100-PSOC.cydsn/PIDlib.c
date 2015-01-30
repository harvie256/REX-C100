#include <project.h>

uint16_t sadd16(uint16_t a, uint16_t b);
uint32_t sadd32(uint32_t a, uint32_t b);
short saddLimit(short a, short b, short lower, short upper);

unsigned char PID_Power_percent;

int intergration;

// Factors are based on the error (x10 * factor) / 100 = power percent
// all are added and limited to the range of 0 - 100 percent

// For proportional Factor, a count of 1 corrasponds to 0.1% of power per degree of temp change
unsigned short proportionalFactor = 100; // 100 will mean 100% power for 10 degrees below target temp
unsigned short intergrationFactor = 30;
unsigned short targetTempX10 = 430;

void PID_Calc(short tempX10)
{
    unsigned char power = 0;
    
    short error = targetTempX10 - tempX10;
    
    intergration = saddLimit(intergration, error, 0, 1000);
    
    // Proportional
    power += ((error * proportionalFactor) / 100);
    // Intergral
    power += (intergrationFactor * intergration) / 1000;
    
    
    if(PID_Power_percent > 100)
    {
        PID_Power_percent = 100;
    }

    PID_Power_percent = power;
}

short saddLimit(short a, short b, short lower, short upper)
{
    short total = a + b;
    
    if(total < lower) return lower;
    if(total > upper) return upper;
    return total;
}

uint16_t sadd16(uint16_t a, uint16_t b)
    { return (a > 0xFFFF - b) ? 0xFFFF : a + b; }

uint32_t sadd32(uint32_t a, uint32_t b)
    { return (a > 0xFFFFFFFF - b) ? 0xFFFFFFFF : a + b;}