
#include <project.h>
#include "Display.h"

#define SYSTICK_INTERRUPT_VECTOR_NUMBER 15u /* Cortex-M0 hard vector */
/* clock and interrupt rates, in Hz */
#define CLOCK_FREQ 24000000u
#define INTERRUPT_FREQ 500u

void uart_init();


CY_ISR(SysTick_ISR)
{
    DisplayScan();
}

void system_init()
{
    /* point the Systick vector to the ISR */
    CyIntSetSysVector(SYSTICK_INTERRUPT_VECTOR_NUMBER, SysTick_ISR);
    /* Set the number of ticks between interrupts.
    Ignore the function success/fail return value */

    (void)SysTick_Config(CLOCK_FREQ / INTERRUPT_FREQ);
    /* Enable Global Interrupt */
    CyGlobalIntEnable; 
    uart_init();
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
    system_init();
    
    SetUpperPoint(ON);
    SetLowerPoint(OFF);
    
    short counter = 0;
    for(;;)
    {
        SetDisplayChars(counter++, displayChars);
        SetLED(LED_AT);
        CyDelay(500);
        //UART_1_UartPutString("this is a string");
        SetDisplayChars(-(counter++), displayChars + 4);
        ClearLED(LED_AT);
        CyDelay(500);
    }
}

