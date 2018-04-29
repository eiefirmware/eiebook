/*!**********************************************************************************************************************
@file sam3u_uart.c                                                                
@brief Provides a driver to use UART peripherals on the SAM3U processor to send and receive data using interrupts. 

This driver covers both the dedicated UART peripheral and the three USART peripherals (assuming they are
running in asynchronous (UART) mode).

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

**********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Uart"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;          /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;           /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;            /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;       /*!< @brief From main.c */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Uart_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Uart_pfnStateMachine;         /*!< @brief The UART application state machine function pointer */

static u32 Uart_u32Timer;                        /*!< @brief Counter used across states */


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UartInitialize(void)

@brief Initializes the UART application and its variables.  

Requires:
- NONE 

Promises:
- 
*/
void UartInitialize(void)
{

  /* Set application pointer */
  Uart_pfnStateMachine = UartSM_Idle;
  
} /* end UartInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void UartRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UartRunActiveState(void)
{
  Uart_pfnStateMachine();

} /* end UartRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UartSM_Idle(void)

@brief Wait for a transmit message to be queued.  Received data is handled in interrupts. 
*/
static void UartSM_Idle(void)
{
  
} /* end UartSM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UartSM_Error(void)

@brief Handle an error
*/
static void UartSM_Error(void)          
{
  
  
} /* end UartSM_Error() */
          
          
          
          
        
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

