
#include <math.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_hibernate.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/hibernate.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
//#include "rgb_commands.h"
#include "drivers/qs-rgb.h"




//*****************************************************************************
//
// Entry counter to track how long to stay in certain staging states before
// making transition into hibernate.
//
//*****************************************************************************
static volatile unsigned long ulHibModeEntryCount;




//*****************************************************************************
//
// Application state structure.  Gets stored to hibernate memory for
// preservation across hibernate events.
//
//*****************************************************************************
volatile sAppState_t g_sAppState;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


//*****************************************************************************
//
// Handler to manage the button press events and state machine transitions
// that result from those button events.
//
// This function is called by the SysTickIntHandler if a button event is
// detected. Function will determine which button was pressed and tweak various
// elements of the global state structure accordingly.
//
//*****************************************************************************
void
AppButtonHandler(void)
{
    static unsigned long ulTickCounter;

    ulTickCounter++;

    //
    // Switch statement to adjust the color wheel position based on buttons
    //
    switch(g_sAppState.ulButtons & ALL_BUTTONS)
    {

    case ALL_BUTTONS:
    	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        }
    }


//*****************************************************************************
//
// Uses the fColorWheelPos variable to update the color mix shown on the RGB
//
// ulForceUpdate when set forces a color update even if a color change
// has not been detected.  Used primarily at startup to init the color after
// a hibernate.
//
// This function is called by the SysTickIntHandler to update the colors on
// the RGB LED whenever a button or timeout event has changed the color wheel
// position.  Color is determined by a series of sine functions and conditions
//
//*****************************************************************************

//*****************************************************************************
//
// Called by the NVIC as a result of SysTick Timer rollover interrupt flag
//
// Checks buttons and calls AppButtonHandler to manage button events.
// Tracks time and auto mode color stepping.  Calls AppRainbow to implement
// RGB color changes.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{


    g_sAppState.ulButtons = ButtonsPoll(0,0);
    AppButtonHandler();



}

//*****************************************************************************
//
// Uses the fColorWheelPos variable to update the color mix shown on the RGB
//
// This function is called when system has decided it is time to enter
// Hibernate.  This will prepare the hibernate peripheral, save the system
// state and then enter hibernate mode.
//
//*****************************************************************************


//*****************************************************************************
//
// Main function performs init and manages system.
//
// Called automatically after the system and compiler pre-init sequences.
// Performs system init calls, restores state from hibernate if needed and
// then manages the application context duties of the system.
//
//*****************************************************************************
int
main(void)
{

    unsigned long ulResetCause;


    //
    // Enable stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    FPUEnable();
    FPUStackingEnable();

    //
    // Set the system clock to run at 40Mhz off PLL with external crystal as
    // reference.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Enable the hibernate module
    //
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

    //
    // Enable and Initialize the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);


    //
    // Determine why system reset occurred and respond accordingly.
    //
    ulResetCause = SysCtlResetCauseGet();
    SysCtlResetCauseClear(ulResetCause);


    //
    // Initialize clocking for the Hibernate module
    //
    HibernateEnableExpClk(SysCtlClockGet());

    //
    // Initialize the RGB LED. AppRainbow typically only called from interrupt
    // context. Safe to call here to force initial color update because
    // interrupts are not yet enabled.
    //

    //
    // Initialize the buttons
    //
    ButtonsInit();

    //
    // Initialize the SysTick interrupt to process colors and buttons.
    //
    SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
    SysTickEnable();
    SysTickIntEnable();
    IntMasterEnable();
while(1){;}
}
