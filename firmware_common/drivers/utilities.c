/*!**********************************************************************************************************************
@file utilities.c                                                                

@brief Various useful functions and definitions.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- bool IsTimeUp(u32 *pu32SavedTick_, u32 u32Period_)

PROTECTED FUNCTIONS
- NONE

***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Util"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                /*!< From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Util_<type>" and be declared as static.
***********************************************************************************************************************/


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------------------------------------------------
@fn bool IsTimeUp(u32 *pu32SavedTick_, u32 u32Period_)
  
@brief Checks if the difference between the current G_u32SystemTime1ms and the 
saved G_u32SystemTime1ms is greater than the period specified. 

The referenced current time is always G_u32SystemTime1ms.  The function 
handles rollover of G_u32SystemTime1ms.

Example
#define U32_PERIOD    (u32)1000

u32 u32Timeout = G_u32SystemTime1ms;

// other code runs for a while
if( IsTimeUp(&u32Timeout, U32_PERIOD) )
{
  // Time is up so do something
}

Requires:
@param *pu32SavedTick_ points to the saved tick value (in ms)
@param u32Period_ is the desired period in ms

Promises:
- Returns FALSE if u32Period_ has not elapsed
- Returns TRUE if u32Period_ has elapsed

*/
bool IsTimeUp(u32 *pu32SavedTick_, u32 u32Period_)
{
  u32 u32TimeElapsed;
  
  /* Check to see if the timer in question has rolled */
  if(G_u32SystemTime1ms >= *pu32SavedTick_)
  {
    u32TimeElapsed = G_u32SystemTime1ms - *pu32SavedTick_;
  }
  else
  {
    u32TimeElapsed = (0xFFFFFFFF - *pu32SavedTick_) + G_u32SystemTime1ms;
  }

  /* Now determine if time is up */
  if(u32TimeElapsed < u32Period_)
  {
    return(FALSE);
  }
  else
  {
    return(TRUE);
  }

} /* end IsTimeUp() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
