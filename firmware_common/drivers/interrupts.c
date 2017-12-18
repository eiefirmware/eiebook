/*!*********************************************************************************************************************
@file interrupts.c                                                               
@brief Definitions for main system interrupts.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- 
PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- 


***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>ISR"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword)  */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variables names shall start with "ISR_<type>" and be declared as static.
***********************************************************************************************************************/


/**********************************************************************************************************************
Interrupt Service Routine Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void InterruptSetup(void)

@brief Disables and clears all NVIC interrupts and sets up interrupt priorities.

Peripheral interrupt sources should be enabled outside of this function.
As this should be the first interrupt-related function that is called in
the system, we can conclude that clearing all the pending flags should
work since no peripheral interrupt sources should be connected yet. 

Requires:
- IRQn_Type enum is the sequentially ordered interrupt values starting at 0

Promises:
- Interrupt priorities are set 
- All NVIC interrupts are disabled and all pending flags are cleared

*/
void InterruptSetup(void)
{
  static const u32 au32PriorityConfig[PRIORITY_REGISTERS] = {IPR0_INIT, IPR1_INIT, IPR2_INIT, 
                                                             IPR3_INIT, IPR4_INIT, IPR5_INIT,
                                                             IPR6_INIT, IPR7_INIT};
  
  /* Disable all interrupts and ensure pending bits are clear */
  for(u8 i = 0; i < U8_SAM3U2_INTERRUPT_SOURCES; i++)
  {
    NVIC_DisableIRQ( (IRQn_Type)i );
    NVIC_ClearPendingIRQ( (IRQn_Type) i);
  } 

  /* Set interrupt priorities */
  for(u8 i = 0; i < PRIORITY_REGISTERS; i++)
  {
    ((u32*)(AT91C_BASE_NVIC->NVIC_IPR))[i] = au32PriorityConfig[i];
  }
      
} /* end InterruptSetup(void) */

  





/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
