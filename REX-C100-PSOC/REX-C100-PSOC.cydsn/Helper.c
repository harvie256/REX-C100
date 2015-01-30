#include <project.h>


void ConvertNumber_Q8_7_To_3_1_Ascii(short value, char* buffer)
{
    int8 i;
    int8 numPos = 4;
    uint16 tmp;

    int32 scale = value;
    scale *= 10;
    scale = (scale >> 7);
    value = scale;
    
    if(value < 0)
    {
        *(buffer + 3) = '-';        
        value = -value;
        numPos = 3;
    }
    
    for(i=0; i < numPos; i++)
    {
        tmp = value % 10;
        value = value / 10;
        *(buffer + i) = '0' + tmp;
    }
}