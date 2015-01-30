
#include <project.h>
#include "main.h"
#include "Display.h"
#include "DS18B20.h"
#include "PIDlib.h"
#include "Helper.h"

void uart_init();
char* itoa(int i, char b[]);
char* itoaFixed(int i, char b[], int numDigits);

unsigned int systemTimer = 0;
unsigned int nextPIDUpdate = 0;
unsigned int nextElementUpdate = 0;
unsigned int PowerElementOffTime = 0;

#define HEATING_ELEMENT_TIME 2000

short temp100;

void ElementOn();
void ElementOff();

CY_ISR(SysTick_ISR)
{
    DisplayScan();
    systemTimer += 2;
    
    if(systemTimer >= nextPIDUpdate)
    {
        PID_Calc(temp100 / 10);
        nextPIDUpdate = systemTimer + PID_CYCLE_TIME_MS;
    }
    
    if(systemTimer >= nextElementUpdate)
    {        
        PowerElementOffTime = systemTimer + (HEATING_ELEMENT_TIME * PID_Power_percent)/100;
        ElementOn();

        nextElementUpdate = systemTimer + HEATING_ELEMENT_TIME;
    }
    
    if(systemTimer >= PowerElementOffTime)
    {
        ElementOff();
    }
    
}

void ElementOn()
{
    PID_out_Write(1);
}

void ElementOff()
{
    PID_out_Write(0);
}

unsigned char system_init()
{
    unsigned char error = 0;
    /* point the Systick vector to the ISR */
    CyIntSetSysVector(SYSTICK_INTERRUPT_VECTOR_NUMBER, SysTick_ISR);
    /* Set the number of ticks between interrupts.
    Ignore the function success/fail return value */
    (void)SysTick_Config(CLOCK_FREQ / INTERRUPT_FREQ);

    
    //Setup peripheral systems
    uart_init();
    error |= DS18B20_init();

    CyGlobalIntEnable;
    return error;
}

// Setup the uart in open collector drive, allowing the 5V PSOC
// to drive 3.3V serial line.
void uart_init()
{
    UART_1_tx_SetDriveMode(UART_1_tx_DM_OD_LO);
    UART_1_Init();
    UART_1_Enable(); 
}

int main()
{

    if(system_init())
    {
        // Problems initialising hardware, endless loop flashing AT light at 5hz
        for(;;)
        {
            SetLED(LED_AT);
            CyDelay(100);
            ClearLED(LED_AT);
            CyDelay(100);
        }
    }
    
    char displayBuffer[8];
    short tempQ8_7;
    // Loop getting and displaying the temp at 1hz
    for(;;)
    {
        DS18B20_initiateConversion();
        CyDelay(1000);
        tempQ8_7 = DS18B20_GetCurrentTempQ8_7();
        ConvertNumber_Q8_7_To_3_1_Ascii(tempQ8_7, displayBuffer + 4);
        SetDisplayChars(displayBuffer);
        SetUpperPoint(ON);

        
        
        
        /*
        
        UART_1_UartPutString("s(");
        UART_1_UartPutString(itoa(temp100/100, stringBuffer));
        UART_1_UartPutString(".");
        UART_1_UartPutString(itoaFixed(temp100%100, stringBuffer,2));
        UART_1_UartPutString(")\x0d\x0a");
        */
    }
    
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

char* itoaFixed(int i, char b[], int numDigits){

    char const digit[] = "0123456789";
    char* p = b + numDigits;
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(p > b);
    return b;
}

