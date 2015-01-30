/*
 * Library to bitbang the DS18B20 temp sensor
 * Code adapted from a cut down version of then
 * Arduino one-wire serial library.
 * http://playground.arduino.cc/Learning/OneWire
*/

#include <project.h>
#include "DS18B20.h"

// Private function prototypes
unsigned char OneWireInByte();
void OneWireOutByte(unsigned char data);
unsigned char OneWireReset();

unsigned char DS18B20_init()
{
    DS18B20_Data_SetDriveMode(DS18B20_Data_DM_OD_LO);
    DS18B20_Data_Write(1); //Put the bus in idle state
    
    // Give time for the power to the peripheral to stabalise
    CyDelay(1);
    return OneWireReset();
    // Note: not setting the config register, 
    // therefore the default startup of 12bit resolution is used.
}

void DS18B20_initiateConversion()
{
    OneWireReset(); 
    OneWireOutByte(0xcc); // Skip ROM command (address all devices on the bus)
    OneWireOutByte(0x44); // perform temperature conversion
}

short DS18B20_GetCurrentTempQ8_7()
{
    unsigned char HighByte, LowByte;
    short TReading;

    OneWireReset(); 
    OneWireOutByte(0xcc);  // Skip ROM command (address all devices on the bus)
    OneWireOutByte(0xbe);  // Read Scratchpad command

    LowByte = OneWireInByte(); 
    HighByte = OneWireInByte(); 
    TReading = (HighByte << 8) + LowByte;

    if (TReading & 0x8000) // negative 
    { 
        TReading = (TReading ^ 0xffff) + 1;   // 2's comp 
    }
    
    // Treading will be in Q7.4 out of the sensor.  12 bits, the lower 4 bits are fractional
    // Convert to Q8.7 by << 3.
    return (TReading << 3);
}

short DS18B20_GetCurrentTempX100() 
{   
    unsigned char HighByte, LowByte;
    short TReading;

    OneWireReset(); 
    OneWireOutByte(0xcc);  // Skip ROM command (address all devices on the bus)
    OneWireOutByte(0xbe);  // Read Scratchpad command

    LowByte = OneWireInByte(); 
    HighByte = OneWireInByte(); 
    TReading = (HighByte << 8) + LowByte;

    if (TReading & 0x8000) // negative 
    { 
        TReading = (TReading ^ 0xffff) + 1;   // 2's comp 
    } 

    return (6 * TReading) + (TReading >> 2);    // multiply by (100 * 0.0625) or 6.25 
}	

unsigned char OneWireReset()
{ 
    short i;
    unsigned char response = 1;
    
    uint8 interrupts = CyEnterCriticalSection();
    
    DS18B20_Data_Write(0);
    CyDelayUs(500); 
    DS18B20_Data_Write(1);
    CyDelayUs(15);
    // The DS18B20 now must respond by pulling the bus low at some point in the next 240us
    for(i=0; i<60; i++)
    {
        CyDelayUs(1);
        if(DS18B20_Data_Read() == 0)
        {
            response = 0;
        }
    }
    
    CyExitCriticalSection(interrupts);
    return response;
} 

void OneWireOutByte(unsigned char data)          
{ 
    int i;
    CyGlobalIntDisable;
    
    for(i=0; i<8; i++)
    {
        if(data & 0x01)
        {
            // High bit
            DS18B20_Data_Write(0);
            CyDelayUs(5);
            DS18B20_Data_Write(1);
            CyDelayUs(60);
        }
        else
        {
            //Low Bit
            DS18B20_Data_Write(0);
            CyDelayUs(65);
            DS18B20_Data_Write(1);
        }
        data = data >> 1;
    }
    CyGlobalIntEnable; 
} 

unsigned char OneWireInByte()
{ 
    int i;
    unsigned char temp, data;
    
    CyGlobalIntDisable; 

    for (i=0; i<8; i++) 
    { 
        DS18B20_Data_Write(0);
        CyDelayUs(5);
        DS18B20_Data_Write(1);
        CyDelayUs(5);
        temp = DS18B20_Data_Read();
        CyDelayUs(50);
        data = (data >> 1) | (temp << 7);
    } 
    CyGlobalIntEnable; 
    return(data); 
}


