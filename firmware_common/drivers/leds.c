/*!**********************************************************************************************************************
@file leds.c                                                                
@brief LED driver and API

This driver provides on, off, toggle, blink and PWM functionality.
The basic on/off/toggle functionality is applied directly to the LEDs.
Blinking and PWMing of LEDs rely on the EIE operating system to provide timing at
regular 1ms calls to LedUpdate().

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- 

PUBLIC FUNCTIONS

PROTECTED FUNCTIONS

***********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Led"
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/* New variables (all shall start with G_xxLed*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Led_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Led_StateMachine;                   /*!< @brief The state machine function pointer */
//static u32 Led_u32Timeout;                           /*!< @brief Timeout counter used across states */

 

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedInitialize(void)

@brief Initialization of LED system parameters and visual LED check.


Requires:
- 

Promises:
- 

*/
void LedInitialize(void)
{
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    Led_StateMachine = LedSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Led_StateMachine = LedSM_Error;
  }
  
} /* end LedInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void LedRunActiveState(void)
{
  Led_StateMachine();

} /* end LedRunActiveState */



/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void LedSM_Idle(void)

@brief What does this function do?
*/
static void LedSM_Idle(void)
{
 
} /* end LedSM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void LedSM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void LedSM_Error(void)          
{
  
} /* end LedSM_Error() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


