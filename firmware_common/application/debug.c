/*!**********************************************************************************************************************
@file debug.c 
@brief Debugging functions and state machine. 

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- 

CONSTANTS
- 

TYPES
- 

PUBLIC FUNCTIONS
- 

PROTECTED FUNCTIONS
- 

***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_xxDebug"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Debug_xx" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Debug_pfnStateMachine;                /*!< @brief The Debug state machine function pointer */
static u32 Debug_u32Timeout;                           /*!< @brief Timeout counter used across states */


/***********************************************************************************************************************
* Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void DebugInitialize(void)

@brief Sets up the debug command list and activates the debug functionality.

Should only be called once in main init section.

Requires:
- The debug application is not yet running

Promises:
- 

*/
void DebugInitialize(void)
{
  
  /* Go to error state if the UartRequest failed */
  if(0)
  {
    Debug_pfnStateMachine = DebugSM_Error;
  }

  /* Otherwise send the first message, set "good" flag and head to Idle */
  else
  {
    Debug_pfnStateMachine = DebugSM_Idle;
  }
  
} /* end  DebugInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void DebugRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer
*/
void DebugRunActiveState(void)
{
  Debug_pfnStateMachine();

} /* end DebugRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Declarations
***********************************************************************************************************************/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void DebugSM_Idle(void)               

@brief  

*/
void DebugSM_Idle(void)               
{
    
} /* end DebugSM_Idle() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void DebugSM_Error(void)         

@brief The Error state for the task.

*/
void DebugSM_Error(void)         
{

} /* end DebugSM_Error() */
             

          
             
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

