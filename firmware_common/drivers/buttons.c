/*!**********************************************************************************************************************
@file buttons.c                                                                
@brief Button functions and state machine.  

The application handles all debouncing and button press / hold detection.
All buttons use interrupts to trigger the start and end
of the action.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- 

TYPES
- NONE


PUBLIC FUNCTIONS
- 

Protected:
- 

***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Button"
***********************************************************************************************************************/
/* New variables */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                     /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                      /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                       /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                  /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Button_xx" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Button_pfnStateMachine;                  /*!< @brief The Button application state machine function pointer */


/************ EDIT BOARD-SPECIFIC GPIO DEFINITIONS ABOVE ***************/


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public Functions */
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected Functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: ButtonInitialize

Description:


Requires:
  - 
 
Promises:
  - 
*/
void ButtonInitialize(void)
{


} /* end ButtonInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function ButtonRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void ButtonRunActiveState(void)
{
  Button_pfnStateMachine();

} /* end ButtonRunActiveState */



/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions

The button state machine monitors button activity and manages debouncing and
maintaining the global button states.
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Do nothing but wait for a debounce time to start */
static void ButtonSM_Idle(void)                
{
  
} /* end ButtonSM_Idle(void) */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void ButtonSM_Error(void)          
{
  
} /* end ButtonSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
