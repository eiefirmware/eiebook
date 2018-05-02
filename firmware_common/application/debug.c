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
All Global variable names shall start with "G_<type>Debug"
***********************************************************************************************************************/
/* New variables */
u32 G_u32DebugFlags;                                   /*!< @brief Debug flag register */

u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE];     /*!< @brief Space to latch characters for DebugScanf() */
u8 G_u8DebugScanfCharCount = 0;                        /*!< @brief Counter for # of characters in Debug_au8ScanfBuffer */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Debug_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Debug_pfnStateMachine;                /*!< @brief The Debug state machine function pointer */

static UartPeripheralType* Debug_Uart;                   /*!< @brief Pointer to debug UART peripheral object */
static u32 Debug_u32CurrentMessageToken;                 /*!< @brief Token for current message */
static u8 Debug_u8ErrorCode;                             /*!< @brief Error code */

static u8 Debug_au8RxBuffer[DEBUG_RX_BUFFER_SIZE];       /*!< @brief Space for incoming characters of debug commands */
static u8 *Debug_pu8RxBufferNextChar;                    /*!< @brief Pointer to next spot in the Rxbuffer */
static u8 *Debug_pu8RxBufferParser;                      /*!< @brief Pointer to loop through the Rx buffer */

static u8 Debug_au8CommandBuffer[DEBUG_CMD_BUFFER_SIZE]; /*!< @brief Space to store chars as they build up to the next command */ 
static u8 *Debug_pu8CmdBufferNextChar;                   /*!< @brief Pointer to incoming char location in the command buffer */
static u16 Debug_u16CommandSize;                         /*!< @brief Number of characters in the command buffer */
static u8 Debug_u8Command;                               /*!< @brief A validated command number */

/*! @brief Add commands by updating debug.h in the Command-Specific Definitions section, then update this list
with the function name to call for the corresponding command: */
#ifdef EIE1
DebugCommandType Debug_au8Commands[DEBUG_COMMANDS] = 
{ {DEBUG_CMD_NAME00, DebugCommandPrepareList},
  {DEBUG_CMD_NAME01, DebugCommandLedTestToggle},
  {DEBUG_CMD_NAME02, DebugCommandSysTimeToggle},
  {DEBUG_CMD_NAME03, DebugCommandDummy},
  {DEBUG_CMD_NAME04, DebugCommandDummy},
  {DEBUG_CMD_NAME05, DebugCommandDummy},
  {DEBUG_CMD_NAME06, DebugCommandDummy},
  {DEBUG_CMD_NAME07, DebugCommandDummy} 
};

static u8 Debug_au8StartupMsg[] = "\n\n\r*** RAZOR SAM3U2 ASCII LCD DEVELOPMENT BOARD ***\n\n\r";
#endif /* EIE1 */

/***********************************************************************************************************************
* Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn u32 DebugPrintf(u8* u8String_)

@brief Queues the string pointed to by u8String_ to the Debug port.  

The string must be null-terminated.  It may also contain control characters 
like newline (\n) and linefeed (\f)

Example:
u8 u8String[] = "A string to print.\n\r"

DebugPrintf(u8String);

Requires:
- The debug UART resource has been setup for the debug application.
- The size of the string will not exceed the total available message
  slots in the system.  As a guideline, this should be less than
  10 x U16_MAX_TX_MESSAGE_LENGTH but this is not enforced.

@param u8String_ is a NULL-terminated C-string

Promises:
- The string is queued to the debug UART.
- The message token is returned

*/
u32 DebugPrintf(u8* u8String_)
{
  u8* pu8Parser = u8String_;
  u32 u32Size = 0;
  
  while(*pu8Parser != '\0') 
  {
    u32Size++;
    pu8Parser++;
  }
    
  return( UartWriteData(Debug_Uart, u32Size, u8String_) );
 
} /* end DebugPrintf() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void DebugLineFeed(void)

@brief Queues a <CR><LF> sequence to the debug UART.  

This is slightly more efficient than calling DebugPrintf("\n\r");


Requires:
- NONE

Promises:
- <CR><LF> sequence is sent to the debug UART

*/
void DebugLineFeed(void)
{
  u8 au8Linefeed[] = {ASCII_LINEFEED, ASCII_CARRIAGE_RETURN};
  
  UartWriteData(Debug_Uart, sizeof(au8Linefeed), &au8Linefeed[0]);

} /* end DebugLineFeed() */


/*!-----------------------------------------------------------------------------/
@fn void DebugPrintNumber(u32 u32Number_)
@brief Formats a long into an ASCII string and queues to print.  

Leading zeros are not printed. Unsigned (positive) values only.

Example:

u32 u32Number = 1234567;

DebugPrintNumber(u32Number);


Requires:
- Enough space is available on the heap to temporarily store the number array

@param u32Number_ is the number to print.

Promises:
- The number is converted to an array of ascii without leading zeros and sent to UART

*/
void DebugPrintNumber(u32 u32Number_)
{
  bool bFoundDigit = FALSE;
  u8 au8AsciiNumber[10];
  u8 u8CharCount = 0;
  u32 u32Temp, u32Divider = 1000000000;
  u8 *pu8Data;
  
  /* Parse out all the digits, start counting after leading zeros */
  for(u8 index = 0; index < 10; index++)
  {
    /* Get the digit and add offset to get ASCII character */
    au8AsciiNumber[index] = (u32Number_ / u32Divider) + NUMBER_ASCII_TO_DEC;
    if(au8AsciiNumber[index] != '0')
    {
      bFoundDigit = TRUE;
    }
    if(bFoundDigit)
    {
      u8CharCount++;
    }
    u32Number_ %= u32Divider;
    u32Divider /= 10;
  }
  
  /* Handle special case where u32Number == 0 */
  if(!bFoundDigit)
  {
    u8CharCount = 1;
  }
  
  /* Allocate memory for the number and copy the array */
  pu8Data = malloc(u8CharCount);
  if (pu8Data == NULL)
  {
    Debug_u8ErrorCode = DEBUG_ERROR_MALLOC;
    Debug_pfnStateMachine = DebugSM_Error;
  }
  
  u32Temp = 9;
  for(u8 index = u8CharCount; index != 0; index--)
  {
    pu8Data[index - 1] = au8AsciiNumber[u32Temp--];
  }
    
  /* Print the ascii string and free the memory */
  UartWriteData(Debug_Uart, u8CharCount, pu8Data);
  free(pu8Data);
  
} /* end DebugDebugPrintNumber() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void DebugInitialize(void)

@brief Sets up the debug command list and activates the debug functionality.

Should only be called once in main init section.

Requires:
- The debug application is not yet running
- The UART resource requested should be free

Promises:
- Debug_au8RxBuffer[] initialized to all 0

@param Debug_pu8CmdBufferCurrentChar set to Debug_au8RxBuffer[0]
@param Debug_pu8RxBufferParser set to Debug_au8RxBuffer[0]
@param Debug_pfnStateMachine set to Idle

*/
void DebugInitialize(void)
{
  u8 au8FirmwareVersion[] = FIRMWARE_VERSION;
  UartConfigurationType sUartConfig;  

  /* Clear the receive buffer and initialize its pointers */
  for (u16 i = 0; i < DEBUG_RX_BUFFER_SIZE; i++)
  {
    Debug_au8RxBuffer[i] = 0;
  }

  Debug_pu8RxBufferParser    = &Debug_au8RxBuffer[0];
  Debug_pu8RxBufferNextChar  = &Debug_au8RxBuffer[0]; 

  /* Clear the scanf buffer and counter */
  G_u8DebugScanfCharCount = 0;
  for (u8 i = 0; i < DEBUG_SCANF_BUFFER_SIZE; i++)
  {
    G_au8DebugScanfBuffer[i] = 0;
  }

  /* Initailze the command array as needed */
  Debug_pu8CmdBufferNextChar = &Debug_au8CommandBuffer[0]; 

  /* Request the UART resource to be used for the Debug application */
  sUartConfig.UartPeripheral     = DEBUG_UART;
  sUartConfig.pu8RxBufferAddress = &Debug_au8RxBuffer[0];
  sUartConfig.pu8RxNextByte      = &Debug_pu8RxBufferNextChar;
  sUartConfig.u16RxBufferSize    = DEBUG_RX_BUFFER_SIZE;
  sUartConfig.fnRxCallback       = DebugRxCallback;
  
  Debug_Uart = UartRequest(&sUartConfig);
  
  /* Go to error state if the UartRequest failed */
  if(Debug_Uart == NULL)
  {
    Debug_pfnStateMachine = DebugSM_Error;

  }
  /* Otherwise send the first message, set "good" flag and head to Idle */
  else
  {
    DebugPrintf(Debug_au8StartupMsg);   
    DebugPrintf(au8FirmwareVersion);
    
    G_u32ApplicationFlags |= _APPLICATION_FLAGS_DEBUG;
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


/*!----------------------------------------------------------------------------------------------------------------------
@fn void DebugRxCallback(void)

@brief Callback function used when character received.

Requires:
- None

Promises:
@param Debug_pu8RxBufferNextChar is advanced safely

*/
void DebugRxCallback(void)
{
  /* Safely advance the NextChar pointer */
  Debug_pu8RxBufferNextChar++;
  if(Debug_pu8RxBufferNextChar == &Debug_au8RxBuffer[DEBUG_RX_BUFFER_SIZE])
  {
    Debug_pu8RxBufferNextChar = &Debug_au8RxBuffer[0];
  }
  
} /* end DebugRxCallback() */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn static void DebugCommandPrepareList(void)

@brief Queues the entire list of debug commands available in the system so 
they will be sent out the debug UART for the user to view.


Requires:
- Message Sender application is running

Promises:
- Command numbers and names of all installed commands are queued to messagesender.

*/
static void DebugCommandPrepareList(void)
{
  u8 au8ListHeading[] = "\n\n\rAvailable commands:\n\r";
  u8 au8CommandLine[DEBUG_CMD_PREFIX_LENGTH + DEBUG_CMD_NAME_LENGTH + DEBUG_CMD_POSTFIX_LENGTH];
  
  /* Write static characters to command list line */
  au8CommandLine[2] = ':';
  au8CommandLine[3] = ' ';
  au8CommandLine[DEBUG_CMD_PREFIX_LENGTH + DEBUG_CMD_NAME_LENGTH] = '\n';
  au8CommandLine[DEBUG_CMD_PREFIX_LENGTH + DEBUG_CMD_NAME_LENGTH + 1] = '\r';
  au8CommandLine[DEBUG_CMD_PREFIX_LENGTH + DEBUG_CMD_NAME_LENGTH + 2] = '\0';

  /* Prepare a nicely formatted list of commands */
  DebugPrintf(au8ListHeading);
  
  /* Loop through the array of commands parsing out the command number
  and printing it along with the command name. */  
  for(u8 i = 0; i < DEBUG_COMMANDS; i++)
  {
    /* Get the command number in ASCII */
    if(i >= 10)
    {
      au8CommandLine[0] = (i / 10) + NUMBER_ASCII_TO_DEC;
    }
    else
    {
      au8CommandLine[0] = NUMBER_ASCII_TO_DEC;
    }
    
    au8CommandLine[1] = (i % 10) + NUMBER_ASCII_TO_DEC;
    
    /* Read the command name */
    for(u8 j = 0; j < DEBUG_CMD_NAME_LENGTH; j++)
    {
      au8CommandLine[DEBUG_CMD_PREFIX_LENGTH + j] = Debug_au8Commands[i].pu8CommandName[j];
    }
    
    /* Queue the command name to the UART */
    DebugPrintf(au8CommandLine);
  }

  DebugLineFeed();
  
} /* end DebugCommandPrepareList() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void DebugCommandDummy(void)

@brief A command place-holder.

Use this function when adding new commands that do not exist yet.  This is 
handy if you are out of space in the command list and might as well add a 
group of new commands instead of just one.  Or perhaps you just need a
temporary place holder.


Requires:
- NONE

Promises:
- A string is printed to tell the user they are using a command 
that does not exist.

*/
static void DebugCommandDummy(void)
{
  u8 au8DummyCommand[] = "\n\rDummy!\n\n\r";
  
  DebugPrintf(au8DummyCommand);
  
} /* end DebugCommandDummy() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void DebugCommandLedTestToggle(void)

@brief Toggles and reports the active state of the LED test.

This implementation is specific to the target hardware.

Requires:
- NONE

Promises:
@param G_u32DebugFlags flag _DEBUG_LED_TEST_ENABLE is toggled

*/
static void DebugCommandLedTestToggle(void)
{
  u8 au8LedTestMessage[] = "\n\rLed Test ";
  
  /* Print message and toggle the flag */
  DebugPrintf(au8LedTestMessage);
  if(G_u32DebugFlags & _DEBUG_LED_TEST_ENABLE)
  {
    G_u32DebugFlags &= ~_DEBUG_LED_TEST_ENABLE;
    DebugPrintf(G_au8UtilMessageOFF);
  }
  else
  {
    G_u32DebugFlags |= _DEBUG_LED_TEST_ENABLE;
    DebugPrintf(G_au8UtilMessageON);
    
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOn(GREEN);
    LedOn(YELLOW);
    LedOn(ORANGE);
    LedOn(RED);
  }
  
} /* end DebugCommandLedTestToggle() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void DebugLedTestCharacter(u8 u8Char_)

@brief Checks the character and toggles associated LED if applicable.

Only responds to UPPER CASE characters.
This implementation is specific to the target hardware.

For EIE1: W, P, B, C, G, Y, O, R

Requires:
@param u8Char_ is the character to check

Promises:
- If u8Char_ is a valid toggling character, the corresponding LED will be toggled.

*/
static void DebugLedTestCharacter(u8 u8Char_)
{
  LedNumberType eLed;
  bool bValidLed = TRUE;
  
  /* Check the char to see if an LED should be toggled */  
  switch (u8Char_)
  {
    case 'W':
    {
      eLed = WHITE;
      break;
    }  

    case 'P':
    {
      eLed = PURPLE;
      break;
    }  

    case 'B':
    {
      eLed = BLUE;
      break;
    }  

    case 'C':
    {
      eLed = CYAN;
      break;
    }  

    case 'G':
    {
      eLed = GREEN;
      break;
    }  

    case 'Y':
    {
      eLed = YELLOW;
      break;
    }  

    case 'O':
    {
      eLed = ORANGE;
      break;
    }  

    case 'R':
    {
      eLed = RED;
      break;
    }  
    
  default:
    {
      bValidLed = FALSE;
      break;
    }
  } /* end switch */
  
  if(bValidLed)
  {
    LedToggle(eLed);
  }
    
} /* end DebugCommandLedTestToggle() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void DebugCommandSysTimeToggle(void)

@brief Toggles the active state of the 1ms violation warning.

Requires:
- NONE

Promises:
@param G_u32DebugFlags flag _DEBUG_TIME_WARNING_ENABLE is toggled

*/
static void DebugCommandSysTimeToggle(void)
{
  u8 au8SysTimeTestMessage[] = "\n\rSystem time violation reporting ";
  
  /* Print message and toggle the flag */
  DebugPrintf(au8SysTimeTestMessage);
  if(G_u32DebugFlags & _DEBUG_TIME_WARNING_ENABLE)
  {
    G_u32DebugFlags &= ~_DEBUG_TIME_WARNING_ENABLE;
    DebugPrintf(G_au8UtilMessageOFF);
  }
  else
  {
    G_u32DebugFlags |= _DEBUG_TIME_WARNING_ENABLE;
    DebugPrintf(G_au8UtilMessageON);
  }
  
} /* end DebugCommandSysTimeToggle() */


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

