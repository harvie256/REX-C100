
#include <project.h>
#include "main.h"
#include "Display.h"
#include "DS18B20.h"

void uart_init();

CY_ISR(SysTick_ISR)
{
    DisplayScan();
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
    
    // Loop getting and displaying the temp at 1hz
    for(;;)
    {
        DS18B20_initiateConversion();
        CyDelay(750);
        short temp100 = DS18B20_GetCurrentTempX100();
        SetDisplayChars((temp100/10), displayChars + 4);
        SetUpperPoint(ON);
    }
    
}

