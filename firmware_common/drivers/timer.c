/*!**********************************************************************************************************************
@file timer.c                                                                
@brief Provide easy access to set up and run a Timer Counter (TC) Peripheral.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- 

PUBLIC FUNCTIONS
- 
PROTECTED FUNCTIONS
- void TimerInitialize(void)
- void TimerRunActiveState(void)

**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Timer"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32TimerFlags;                      /*!< @brief Global Timer state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;            /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;             /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;              /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;         /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Timer_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Timer_StateMachine;            /*!< @brief The state machine function pointer */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void TimerInitialize(void)

@brief Initializes the State Machine and its variables.

Requires:
- NONE

Promises:
- 

*/
void TimerInitialize(void)
{
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    Timer_StateMachine = TimerSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Timer_StateMachine = TimerSM_Error;
  }

} /* end TimerInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void TimerRunActiveState(void)
{
  Timer_StateMachine();

} /* end TimerRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
  

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TimerSM_Idle(void)

@brief Wait for a message to be queued 
*/static void TimerSM_Idle(void)
{
   
} /* end TimerSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void TimerSM_Error(void)          
{
  
} /* end TimerSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
