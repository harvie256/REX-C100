

#include <project.h>
#include "Display.h"

// Private Prototypes
uint8 segmentDecoder(char character);
void SW_Handler();
void pulse_CP();
void pulse_CPn(uint8 pulses);

// Extern variable
unsigned char displayChars[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Private variables
uint8 muxState = 0;
uint8 LEDs_State = 0xFF;
uint8 pressedSW = 0x00;

void SetLED(uint8 led)
{
    LEDs_State &= ~(1<<led);
}

void ClearLED(uint8 led)
{
    LEDs_State |= (1<<led);
}

int SetDisplayChars(short value, unsigned char* array)
{
    int8 i;
    int8 numPos = 4;
    uint16 tmp;
    if(value > 9999) return 1; // Too high to display on 4 segments
    if(value < 0)
    {
        if(value < -999) return 1; // Too low to display with the minus sign
        
        *(array + 3) = segmentDecoder('-');
        value = -value;
        numPos = 3;
    }
    
    for(i=0; i < numPos; i++)
    {
        tmp = value % 10;
        value = value / 10;
        *(array + i) = segmentDecoder('0' + tmp);
    }
    return 0;
}

void DisplayScan()
{
    int i;
    
    switch(muxState)
    {
        case 0:
            DS_Write(1);
            pulse_CP();
            DS_Write(0);
            Seg_Write(displayChars[0]);
            muxState++;
            break;
        case 1 ... 7:
            pulse_CP();
            Seg_Write(displayChars[muxState]);
            muxState++;
            break;
        case 8:
            pulse_CP();
            Lights_COM_Write(1);
            Seg_Write(LEDs_State);
            muxState++;
            break;
        case 9:
            Lights_COM_Write(0);
            Seg_Write(0xFF);
            muxState = 0;
            
            pressedSW = 0x00;
            // At this point all mux lines are low, so check if any sw is pressed
            if(SW_COM_Read() != 0)
            {
                // No switch is pressed since it's pulled high
                break;
            }
            
            DS_Write(1);
            pulse_CPn(4);
            DS_Write(0);
            pulse_CP();
            DS_Write(1);
            // Q0 is now the only SW enabled
            
            for(i = 0; i<4; i++)
            {
                if(SW_COM_Read() == 0)
                {
                    pressedSW |= (1<<i);
                }
                pulse_CP();
            }
            
            // Reset the mux lines to low
            DS_Write(0);
            pulse_CPn(8);
            
            // Do something with the sw value
            SW_Handler();
            break;
    }
}

void SetUpperPoint(unsigned char state)
{
    if(state) {
        displayChars[5] &= ~(1<<0);    // Set upper point
    } else {
        displayChars[5] |= (1<<0);
    }
}

void SetLowerPoint(unsigned char state)
{
    if(state) {
        displayChars[1] &= ~(1<<0);    // Set Lower point
    } else {
        displayChars[1] |= (1<<0);
    }
}

void SW_Handler()
{
    switch(pressedSW)
    {
        case (1<<0):
            displayChars[0] = segmentDecoder('0');
            break;
        case (1<<1):
            displayChars[0] = segmentDecoder('1');
            break;
        case (1<<2):
            displayChars[0] = segmentDecoder('2');
            break;
        case (1<<3):
            displayChars[0] = segmentDecoder('3');
            break;
    }
}

uint8 segmentDecoder(char character)
{
    switch(character)
    {
        case ' ':
            break;
        case '-':
            return 0xFD;
        case '0':
            return 0x03;
            break;
        case '1':
            return 0x9F;
            break;
        case '2':
            return 0x25;
            break;
        case '3':
            return 0x0D;
            break;
        case '4':
            return 0x99;
            break;
        case '5':
            return 0x49;
            break;
        case '6':
            return 0x41;
            break;
        case '7':
            return 0x1F;
            break;
        case '8':
            return 0x01;
            break;
        case '9':
            return 0x09;
            break;
        case 'a':
            return 0x11;
            break;
        case 'b':
            return 0xC1;
            break;
        case 'c':
            return 0x63;
            break;
        case 'd':
            return 0x85;
            break;
        case 'e':
            return 0x61;
            break;
        case 'f':
            return 0x71;
            break;
        default:
            break;
    }
    return 0xFF;
}

void pulse_CP()
{
    CP_Write(1);
    CP_Write(0);
}

void pulse_CPn(uint8 pulses)
{
    uint8 count;
    for(count=0; count < pulses; count++)
    {
        pulse_CP();
    }
}