/*!**********************************************************************************************************************
@file sam3u_i2c.c                                                                
@brief MASTER ONLY.  Provides a driver to use TWI0 (IIC/I2C) peripheral to send and receive data using 
interrupts and PDC direct memory access.

Currently Set at 200kHz Master Mode.

Due to the nature of I2C use-cases, this driver does not require tasks to request and release it.
Read / write messages information is queued locally with all required details.  The driver will
continually cycle through the local message buffer and perform the reads or writes on a FIFO basis.
Read messages stand alone.  Write messages will have associated Message task messages.

Clock stretching is supported automatically by the peripheral in Master mode for both read and write.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- G_u32Twi0ApplicationFlags

CONSTANTS
- NONE

TYPES
- TwiStopType
- TwiDirectionType
- TwiPeripheralType
- TwiMessageQueueType

PUBLIC FUNCTIONS
- bool TwiReadData(u8 u8SlaveAddress_, u8* pu8RxBuffer_, u32 u32Size_)
- u32 TwiWriteData(u8 u8SlaveAddress_, u32 u32Size_, u8* u8Data_, TwiStopType Send_)

PROTECTED FUNCTIONS
- void SspInitialize(void)
- void SspRunActiveState(void)
- void SspManualMode(void)
- void SSP0_IRQHandler(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Twi"
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
Variable names shall start with "TWI_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type TWI_pfnStateMachine;           /*!< @brief The application state machine */

static u32 TWI_u32Timer;                          /*!< @brief Timeout counter used across states */
static u32 TWI_u32Flags;                          /*!< @brief Application flags */

static TwiPeripheralType TWI_Peripheral0;         /*!< @brief TWI0 peripheral object */

static TwiMessageQueueType TWI_asMessageBuffer[U8_TWI_MSG_BUFFER_SIZE]; /*!< @brief Local circular buffer for TWI msgs */
static TwiMessageQueueType* TWI_psMsgBufferNext;                        /*!< @brief Next position to place a message */
static TwiMessageQueueType* TWI_psMsgBufferCurrent;                     /*!< @brief Current message that is being processed */
static u8 TWI_u8MsgQueueCount;                                          /*!< @brief Counter to track the number of messages in the queue */


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn bool TwiReadData(u8 u8SlaveAddress_, u8* pu8RxBuffer_, u32 u32Size_)

@brief Queues a TWI Read Message into TWI_asMessageBuffer

Read operations do not have an associated message in the Message task queue.

Requires:
- Master mode

@param u8SlaveAddress_ holds the target's I²C address
@param pu8RxBuffer_ has the space to save the data
@param u32Size_ is the number of bytes to receive

Promises:
- Queues a multi byte command into the command array
- Returns TRUE if the queue was successful

*/
bool TwiReadData(u8 u8SlaveAddress_, u8* pu8RxBuffer_, u32 u32Size_)
{
  
} /* end TwiReadData() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn u32 TwiWriteData(u8 u8SlaveAddress_, u32 u32Size_, u8* u8Data_, TwiStopType eSend_)

@brief Queues a data array for transfer on the TWI0 peripheral.  

Requires:
- if a transmission is in progress, the node in the buffer that is currently being sent will not 
  be destroyed during this function.

@param u8SlaveAddress_ holds the target's I²C address
@param u8Byte_ is the byte to send
@param eSend_ is the type of operation

Promises:
- adds the data message at TWI_Peripheral0.pTransmitBuffer buffer that will be sent by the TWI application
  when it is available.
- Returns the message token assigned to the message; 0 is returned if the message cannot be queued in which case
  G_u32MessagingFlags can be checked for the reason

*/
u32 TwiWriteData(u8 u8SlaveAddress_, u32 u32Size_, u8* u8Data_, TwiStopType eStop_)
{
  
} /* end TwiWriteData() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void TwiInitialize(void)

@brief Initializes the TWI application and its variables. 

Requires:
- NONE 

Promises:
- TWI peripheral objects are ready 
- TWI application set to Idle

*/
void TwiInitialize(void)
{
  
} /* end TwiInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void TwiRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void TwiRunActiveState(void)
{
  TWI_pfnStateMachine();

} /* end TwiRunActiveState */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void TwiManualMode(void)

@brief Runs a transmit cycle of the TWI application to clock a message.  
This function is used only during initialization.

Requires:
- G_u32SystemFlags _SYSTEM_INITIALIZING is set
- TWI application has been initialized.

Promises:
- All pending messages sent
- TWI_u8MsgQueueCount = 0
    
*/
void TwiManualMode(void)
{
  u32 u32Timer;
  
  TWI_u32Flags |=_TWI_INIT_MODE;
  
  while(TWI_u32Flags &_TWI_INIT_MODE)
  {
    TWI_pfnStateMachine();
    MessagingRunActiveState();
    DebugRunActiveState();
    
    u32Timer = G_u32SystemTime1ms;
    while( !IsTimeUp(&u32Timer, 1) );
  }
      
} /* end TwiManualMode() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn ISR void TWI0_IrqHandler(void)

@brief Handles the TWI0 Peripheral interrupts

Requires:
- NONE

Promises:
- NACK: flags error, disables ENDTX and sets Error state
- ENDTX: disables interrupt & PDC, writes STOP (if applicable), and clears _TWI_TRANSMITTING
- ENDRX: disables interrupt & PDC and writes STOP

*/
void TWI0_IrqHandler(void)
{
  
} /* end TWI0_IrqHandler() */


/*----------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*----------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_Idle(void)
@brief Wait for a message to be queued then process that message.
*/
static void TwiSM_Idle(void)
{

} /* end TwiSM_Idle() */
     

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_Transmit(void)
@brief Transmit in progress until until ISR clears _TWI_TRANSMITTING.
*/
static void TwiSM_Transmit(void)
{
    
} /* end TwiSM_Transmit() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_TxWaitComplete(void)
@brief Optional state to wait for the TXCOMP bit to be set (indicates STOP condition
has been placed on the bus). Some Master transmit states will bypass this. */
static void TwiSM_TxWaitComplete(void)
{

} /* end TwiSM_TxWaitComplete() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_PdcReceive(void)
@brief Receive in progress through PDC until 1 byte remains.
The ENDRX ISR is responsible for changing states to exit here.
*/
static void TwiSM_PdcReceive(void)
{
     
} /* end TwiSM_PdcReceive() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_ReceiveLastByte(void)
@brief Wait for RXRDY on the last byte transfer.
*/
static void TwiSM_ReceiveLastByte(void)
{
  
} /* end TwiSM_ReceiveLastByte() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_ReceiveComplete(void)
@brief Wait for final TXCOMP flag
*/
static void TwiSM_ReceiveComplete(void)
{
     
} /* end TwiSM_ReceiveComplete() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_NextTransferDelay(void)
@brief Provide a delay before next transfer starts then do final clean-up before Idle. 
*/
static void TwiSM_NextTransferDelay(void)          
{
  
} /* TwiSM_NextTransferDelay */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TwiSM_Error(void)
@brief Handle an error 
*/
static void TwiSM_Error(void)          
{

} /* end TwiSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/