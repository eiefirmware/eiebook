/*!**********************************************************************************************************************
@file sam3u_spi.c                                                                
@brief Provides a driver to use the dedicated SPI peripheral to send and 
receive data using interrupts.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- G_u32Spi0ApplicationFlags

CONSTANTS
- NONE

TYPES
- SpiBitOrderType
- SpiModeType
- SpiRxStatusType
- SpiConfigurationType
- SpiPeripheralType

PUBLIC FUNCTIONS
- SpiPeripheralType* SpiRequest(SpiConfigurationType* psSpiConfig_)
- void SpiRelease(SpiPeripheralType* psSpiPeripheral_)
- void SpiAssertCS(SpiPeripheralType* psSpiPeripheral_)
- void SpiDeAssertCS(SpiPeripheralType* psSpiPeripheral_)
- u32 SpiWriteByte(SpiPeripheralType* psSpiPeripheral_, u8 u8Byte_)
- u32 SpiWriteData(SpiPeripheralType* psSpiPeripheral_, u32 u32Size_, u8* pu8Data_)

Master mode only:
- bool SpiReadByte(SpiPeripheralType* psSpiPeripheral_)
- bool SpiReadData(SpiPeripheralType* psSpiPeripheral_, u16 u16Size_)
- SpiRxStatusType SpiQueryReceiveStatus(SpiPeripheralType* psSpiPeripheral_)

PROTECTED FUNCTIONS
- void SpiInitialize(void)
- void SpiRunActiveState(void)
- void SpiManualMode(void)
- void SPI0_IRQHandler(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Spi"
***********************************************************************************************************************/
/* New variables */
u32 G_u32Spi0ApplicationFlags;                   /*!< @brief Status flags meant for application using this SPI peripheral */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;          /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;           /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;            /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;       /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "SPI_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Spi_pfnStateMachine;          /*!< @brief The SPI application state machine */

static u32 SPI_u32Timer;                         /*!< @brief Timeout counter used across states */
static u32 SPI_u32Flags;                         /*!< @brief Application flags for SPI */

static SpiPeripheralType SPI_Peripheral0;        /*!< @brief SPI peripheral object */

static u8 SPI_u8Dummies = SPI_DUMMY_BYTE;        /*!< @brief Dummy source byte */


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------------------------------------------------
@fn SpiPeripheralType* SpiRequest(SpiConfigurationType* psSpiConfig_)

@brief Requests access to an SPI resource.  

If the resource is available, the transmit and receive parameters are set up
and the peripheral is made ready to use in the application. The peripheral will be 
configured in different ways for different SPI modes.  The following modes are supported:

SPI_MASTER: transmit and receive using peripheral registers on byte-wise basis using 
interrupts.  Transmit is initiated through Message task.  Receive is based on
queued Rx bytes.  Master receive is non-circular.  The Rx buffer is initialized
to SPI_DUMMY_BYTE and is the source for the transmit dummies.

SPI_SLAVE: transmit through peripheral registers on byte-wise basis using interrupts.
Transmit is initiated through Message.  Receive set up per-byte using peripheral
registers and interrupts and assumes a circular Rx buffer.

Requires:
- SPI peripheral register initialization values in configuration.h must be set 
  correctly; currently this does not support different SPI configurations 
  for multiple Slaves on the same bus - all peripherals on the bus must work with
  the same setup.

@param psSpiConfig_ has the SPI peripheral number, address of the RxBuffer and the RxBuffer size

Promises:
- Returns a pointer to the requested SPI peripheral object if the resource is available; otherwise returns NULL
- Peripheral is enabled
- Peripheral interrupts are enabled as required for the selected mode

*/
SpiPeripheralType* SpiRequest(SpiConfigurationType* psSpiConfig_)
{
  /* If the peripheral is already assigned, return NULL now */
  if( (SPI_Peripheral0->u32PrivateFlags) & _SPI_PERIPHERAL_ASSIGNED )
  {
    return(NULL);
  }

  /* Activate and configure the peripheral */
  AT91C_BASE_PMC->PMC_PCER |= (1 << SPI_Peripheral0->u8PeripheralId);
  
  SPI_Peripheral0->pCsGpioAddress   = psSpiConfig_->pCsGpioAddress;
  SPI_Peripheral0->u32CsPin         = psSpiConfig_->u32CsPin;
  SPI_Peripheral0->eBitOrder        = psSpiConfig_->eBitOrder;
  SPI_Peripheral0->eSpiMode         = psSpiConfig_->eSpiMode;
  SPI_Peripheral0->pu8RxBuffer      = psSpiConfig_->pu8RxBufferAddress;
  SPI_Peripheral0->ppu8RxNextByte   = psSpiConfig_->ppu8RxNextByte;
  SPI_Peripheral0->u16RxBufferSize  = psSpiConfig_->u16RxBufferSize;
  SPI_Peripheral0->u32PrivateFlags |= _SPI_PERIPHERAL_ASSIGNED;
   
  SPI_Peripheral0->pBaseAddress->SPI_CR  = SPI0_CR_INIT;
  SPI_Peripheral0->pBaseAddress->SPI_MR  = SPI0_MR_INIT;
  SPI_Peripheral0->pBaseAddress->SPI_IER = SPI0_IER_INIT;
  SPI_Peripheral0->pBaseAddress->SPI_IDR = SPI0_IDR_INIT;

  SPI_Peripheral0->pBaseAddress->SPI_CSR[0] = SPI0_CSR0_INIT;
  SPI_Peripheral0->pBaseAddress->SPI_CSR[1] = SPI0_CSR1_INIT;
  SPI_Peripheral0->pBaseAddress->SPI_CSR[2] = SPI0_CSR2_INIT;
  SPI_Peripheral0->pBaseAddress->SPI_CSR[3] = SPI0_CSR3_INIT;
  
  /* Special considerations for SPI Slaves */
  if(SPI_Peripheral0->eSpiMode == SPI_SLAVE)
  {
    /* Initialize the receive buffer to dummies */
    for (u16 i = 0; i < SPI_Peripheral0->u16RxBufferSize; i++)
    {
      *(SPI_Peripheral0->pu8RxBuffer + i) = SPI_DUMMY_BYTE;
    }

    /* Set up a dummy transfer */
    SPI_Peripheral0->pu8CurrentTxData = SPI_Peripheral0->pu8RxBuffer;
    SPI_Peripheral0->pBaseAddress->SPI_TDR = *SPI_Peripheral0->pu8CurrentTxData;

    /* Enable the transmit and receive interrupts so they are ready if the Master starts clocking */
    SPI_Peripheral0->pBaseAddress->SPI_IER = (AT91C_SPI_TDRE | AT91C_SPI_RDRF);
  }
  
  /* Enable SPI interrupts */
  NVIC_ClearPendingIRQ( (IRQn_Type)SPI_Peripheral0->u8PeripheralId );
  NVIC_EnableIRQ( (IRQn_Type)SPI_Peripheral0->u8PeripheralId );
  
  return(SPI_Peripheral0);
  
} /* end SpiRequest() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void SpiRelease(SpiPeripheralType* psSpiPeripheral_)

@brief Releases an SPI resource.  

Requires:
- Receive operation is not in progress

@param psSpiPeripheral_ has the SPI peripheral number, address of the RxBuffer, 
and the RxBuffer size.

Promises:
- Resets peripheral object's pointers and data to safe values
- Peripheral is disabled
- Peripheral interrupts are disabled.

*/
void SpiRelease(SpiPeripheralType* psSpiPeripheral_)
{
  /* Check to see if the peripheral is already released */
  if( !(psSpiPeripheral_->u32PrivateFlags) & _SPI_PERIPHERAL_ASSIGNED )
  {
    return;
  }
  
  /* Disable interrupts */
  NVIC_DisableIRQ( (IRQn_Type)(psSpiPeripheral_->u8PeripheralId) );
  NVIC_ClearPendingIRQ( (IRQn_Type)(psSpiPeripheral_->u8PeripheralId) );
 
  /* Now it's safe to release all of the resources in the target peripheral */
  psSpiPeripheral_->pCsGpioAddress  = NULL;
  psSpiPeripheral_->pu8RxBuffer     = NULL;
  psSpiPeripheral_->ppu8RxNextByte  = NULL;
  psSpiPeripheral_->u32PrivateFlags = 0;
  
  /* Empty the transmit buffer if there were leftover messages */
  while(psSpiPeripheral_->psTransmitBuffer != NULL)
  {
    UpdateMessageStatus(psSpiPeripheral_->psTransmitBuffer->u32Token, ABANDONED);
    DeQueueMessage(&psSpiPeripheral_->psTransmitBuffer);
  }
  
} /* end SpiRelease() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn u32 SpiWriteByte(SpiPeripheralType* psSpiPeripheral_, u8 u8Byte_)

@brief Queues a single byte for transfer on the target SPI peripheral.  

Requires:
@param psSpiPeripheral_ is the SPI peripheral to use and it has already been requested.
@param u8Byte_ is the byte to send

Promises:
- Creates a 1-byte message at psSpiPeripheral_->psTransmitBuffer that will be sent 
  by the SPI application when it is available.
- Returns the message token assigned to the message; 0 is returned if the message 
  cannot be queued in which case G_u32MessagingFlags can be checked for the reason

*/
u32 SpiWriteByte(SpiPeripheralType* psSpiPeripheral_, u8 u8Byte_)
{
  u32 u32Token;
  u8 u8Data = u8Byte_;
  
  /* Attempt to queue message and get a response token */
  u32Token = QueueMessage(&psSpiPeripheral_->psTransmitBuffer, 1, &u8Data);
  if( u32Token != 0 )
  {
    /* If the system is initializing, we want to manually cycle the SPI task through one iteration
    to send the message */
    if(G_u32SystemFlags & _SYSTEM_INITIALIZING)
    {
      SpiManualMode();
    }
  }
  
  return(u32Token);
  
} /* end SpiWriteByte() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn u32 SpiWriteData(SpiPeripheralType* psSpiPeripheral_, u32 u32Size_, u8* pu8Data_)

@brief Queues a data array for transfer on the target SPI peripheral.  

Requires:
@param psSpiPeripheral_ is the SPI peripheral to use and it has already been requested.
@param u32Size_ is the number of bytes in the data array
@param u8Data_ points to the first byte of the data array

Promises:
- adds the data message at psSpiPeripheral_->psTransmitBuffer that will be sent by the SPI application
  when it is available.
- Returns the message token assigned to the message; 0 is returned if the message 
  cannot be queued in which case G_u32MessagingFlags can be checked for the reason

*/
u32 SpiWriteData(SpiPeripheralType* psSpiPeripheral_, u32 u32Size_, u8* pu8Data_)
{
  u32 u32Token;

  /* Check for a valid size */
  if(u32Size_ == 0)
  {
    return NULL;
  }

  /* Attempt to queue message and get a response token */
  u32Token = QueueMessage(&psSpiPeripheral_->psTransmitBuffer, u32Size_, pu8Data_);
  if( u32Token == 0 )
  {
    return(0);
  }
  
  /* If the system is initializing, manually cycle the SPI task through one iteration to send the message */
  if(G_u32SystemFlags & _SYSTEM_INITIALIZING)
  {
    SpiManualMode();
  }

  return(u32Token);

} /* end SpiWriteData() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn bool SpiReadByte(SpiPeripheralType* psSpiPeripheral_)

@brief Master mode only.  Queues a request for a single byte from the Slave on 
the target SPI peripheral.  There cannot be pending writes.

Requires:
- Master mode 

@param psSpiPeripheral_ is the SPI peripheral to use and it has already been requested.

Promises:
- Creates a message with one SPI_DUMMY_BYTE at psSpiPeripheral_->psTransmitBuffer that will be sent by the SPI application
  when it is available and thus clock in a received byte to the target receive buffer.
- Returns TRUE and loads the target SPI u16RxBytes

*/
bool SpiReadByte(SpiPeripheralType* psSpiPeripheral_)
{
  /* Confirm Master Mode */
  if( (psSpiPeripheral_->eSpiMode) == SPI_SLAVE )
  {
    return FALSE;
  }

  /* Make sure no Tx or Rx function is already in progress */
  if( (psSpiPeripheral_->u16RxBytes != 0) || (psSpiPeripheral_->psTransmitBuffer != NULL) )
  {
    return FALSE;
  }
  
  /* Load the counter and return success */
  psSpiPeripheral_->u16RxBytes = 1;
  return TRUE;
  
} /* end SpiReadByte() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn bool SpiReadData(SpiPeripheralType* psSpiPeripheral_, u16 u16Size_)

@brief Master mode only. Prepares to get multiple bytes from the Slave on the target SPI peripheral.  

Requires:
- Master mode 

@param psSpiPeripheral_ is the SPI peripheral to use and it has already been requested.
@param u32Size_ is the number of bytes to receive

Promises:
- Returns FALSE if the message is too big, or the peripheral already has a read request
- Returns TRUE and loads the target SPI u16RxBytes

*/
bool SpiReadData(SpiPeripheralType* psSpiPeripheral_, u16 u16Size_)
{
  /* Confirm Master Mode */
  if( (psSpiPeripheral_->eSpiMode) == SPI_SLAVE)
  {
    return FALSE;
  }

  /* Make sure no Tx or Rx function is already in progress */
  if( (psSpiPeripheral_->u16RxBytes != 0) || (psSpiPeripheral_->psTransmitBuffer != NULL) )
  {
    return FALSE;
  }

  /* Do not allow if requested size is too large */
  if(u16Size_ > U16_MAX_TX_MESSAGE_LENGTH)
  {
    DebugPrintf("\r\nSPI message too large\n\r");
    return FALSE;
  }
  
  /* Load the counter and return success */
  psSpiPeripheral_->u16RxBytes = u16Size_;
  return TRUE;
    
} /* end SpiReadData() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn SpiRxStatusType SpiQueryReceiveStatus(SpiPeripheralType* psSpiPeripheral_)

@brief Master mode only.  Returns status of currently requested receive data.

Requires:
@param psSpiPeripheral_ is the SPI peripheral to use and it has already been requested.

Promises:
- Returns the SpiRxStatusType status of the currently receiving message.

*/
SpiRxStatusType SpiQueryReceiveStatus(SpiPeripheralType* psSpiPeripheral_)
{
  /* Confirm Master Mode */
  if( (psSpiPeripheral_->eSpiMode) == SPI_SLAVE)
  {
    return SPI_RX_INVALID;
  }

  /* Check for no current bytes queued */
  if(psSpiPeripheral_->u16RxBytes == 0)
  {
    /* If a transfer just finished and has not be queried... */
    if( psSpiPeripheral_->u32PrivateFlags & _SPI_PERIPHERAL_RX_COMPLETE)
    {
      psSpiPeripheral_->u32PrivateFlags &= ~_SPI_PERIPHERAL_RX_COMPLETE;
      return SPI_RX_COMPLETE;
    }
    /* Otherwise it's just empty */
    else
    {
      return SPI_RX_EMPTY;
    }
  }
  /* If there are bytes waiting, check if waiting or in progress */
  else
  {
    if(psSpiPeripheral_->u32PrivateFlags & _SPI_PERIPHERAL_RX)
    {
      return SPI_RX_RECEIVING;
    }
    else
    {
      return SPI_RX_WAITING;
    }
  }
  
} /* end SpiQueryReceiveStatus() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void SpiInitialize(void)

@brief Initializes the SPI application and its variables.  

The peripheral is not configured until requested by a calling application.

Requires:
- NONE

Promises:
- SPI peripheral object is ready 
- SPI application set to Idle

*/
void SpiInitialize(void)
{
  /* Initialize the SPI peripheral structures */
  SPI_Peripheral0.pBaseAddress     = AT91C_BASE_SPI0;
  SPI_Peripheral0.u8PeripheralId   = AT91C_ID_SPI0;
  SPI_Peripheral0.pCsGpioAddress   = NULL;
  SPI_Peripheral0.psTransmitBuffer = NULL;
  SPI_Peripheral0.pu8RxBuffer      = NULL;
  SPI_Peripheral0.u16RxBufferSize  = 0;
  SPI_Peripheral0.ppu8RxNextByte   = NULL;
  SPI_Peripheral0.u32PrivateFlags  = 0;

  /* Clear all flags */
  SPI_u32Flags = 0;
  G_u32Spi0ApplicationFlags = 0;
  
  /* Set application pointer */
  Spi_pfnStateMachine = SpiSM_Idle;
  DebugPrintf("SPI Peripheral Ready\n\r");

} /* end SpiInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void SpiRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void SpiRunActiveState(void)
{
  Spi_pfnStateMachine();

} /* end SpiRunActiveState */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void SpiManualMode(void)

@brief Runs a transmit cycle of the SPI application to clock out a message.  This function is used only during
initialization.

Requires:
- G_u32SystemFlags _SYSTEM_INITIALIZING is set
- SPI application has been initialized.

Promises:
- All currently queued SPI Master transmit and receive operations are completed.

*/
void SpiManualMode(void)
{
  /* Set up for manual mode */
  SPI_u32Flags |= _SPI_MANUAL_MODE;

  /* Run the SPI state machine so all SPI peripherals send their current message */  
  while(SPI_u32Flags & _SPI_MANUAL_MODE)
  {
    Spi_pfnStateMachine();
    MessagingRunActiveState();
    
    SPI_u32Timer = G_u32SystemTime1ms;
    while( !IsTimeUp(&SPI_u32Timer, 1) );
  }
      
} /* end SpiManualMode() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn ISR void SPI0_IrqHandler(void)

@brief Handler for expected SPI interrupts

Requires:
- None

Promises:
- Each interrupt has different outcomes for the system depending on the SPI mode.  
  See each section for more details.

*/
static void SPI0_IrqHandler(void)
{
  u32 u32Byte;
  u32 u32Timeout;
  u32 u32Current_CSR;

  /* Get a copy of CSR because reading it changes it */
  u32Current_SR = SPI_Peripheral0->pBaseAddress->SPI_SR;

  /*** SPI ISR transmit handling (TDRE) for Master and Slave ***/
  if( (SPI_Peripheral0->pBaseAddress->SPI_IMR & AT91C_SPI_TDRE) && 
      (u32Current_CSR & AT91C_SPI_TDRE) )
  {
    /* Decrement counter and read the dummy byte so the SPI peripheral doesn't overrun */
    SPI_Peripheral0->u32CurrentTxBytesRemaining--;
    u32Byte = SPI_psCurrentISR->pBaseAddress->US_RHR;
    
    if(SPI_Peripheral0->u32CurrentTxBytesRemaining != 0)
    {
      /* Advance the pointer (non-circular buffer)and load the next byte */
      SPI_Peripheral0->pu8CurrentTxData++;
      u32Byte = 0x000000FF & *SPI_psCurrentISR->pu8CurrentTxData;

      /* If we need LSB first, use inline assembly to flip bits with a single instruction. */
      if(SPI_Peripheral0->eBitOrder == SPI_LSB_FIRST)
      {
        u32Byte = __RBIT(u32Byte) >> 24;
      }
    
      /* Load register and clear interrupt flag */
      SPI_Peripheral0->pBaseAddress->SPI_TDR = (u8)u32Byte; 
    }
    else
    {
      /* Done! Disable TX interrupt */
      SPI_Peripheral0->pBaseAddress->US_IDR = AT91C_US_TXEMPTY;
      
      /* Clean up the message status and flags */
      *SPI_pu32SpiApplicationFlagsISR |= _SPI_TX_COMPLETE; 
      UpdateMessageStatus(SPI_psCurrentISR->psTransmitBuffer->u32Token, COMPLETE);
      DeQueueMessage(&SPI_psCurrentISR->psTransmitBuffer);
      SPI_psCurrentISR->u32PrivateFlags &= ~_SPI_PERIPHERAL_TX;  
 
      /* Re-enable Rx interrupt and make final call to callback */    
      SPI_psCurrentISR->pBaseAddress->US_IER = AT91C_US_RXRDY;
    }
    
    /* Both cases use the callback */
    SPI_psCurrentISR->fnSlaveTxFlowCallback();
    
  } /* end AT91C_US_TXEMPTY */

  
} /* end SPI0_IrqHandler() */



/*----------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*----------------------------------------------------------------------------------------------------------------------*/




/***********************************************************************************************************************
State Machine Function Definitions

The SPI state machine monitors messaging activity on the SPI Master peripheral.  
It manages all SPI outgoing messages and will transmit any message that has been queued.  
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void SpiSM_Idle(void)

@brief Wait for a transmit message to be queued -- this can include a dummy transmission 
to receive bytes.
Half duplex transmissions are always assumed. Check one peripheral per iteration. 

*/
static void SpiSM_Idle(void)
{
  if( ( (SPI_Peripheral0->psTransmitBuffer != NULL) || (SPI_Peripheral0->u16RxBytes !=0) ) && 
     !(SPI_Peripheral0->u32PrivateFlags & (_SPI_PERIPHERAL_TX | _SPI_PERIPHERAL_RX)       ) 
    )
  {
    /* Check if the message is receiving based on expected byte count */
    if(SPI_Peripheral0->u16RxBytes !=0)
    {
      /* Receiving: flag that the peripheral is now busy */
      SPI_Peripheral0->u32PrivateFlags |= _SPI_PERIPHERAL_RX;    
      
      /* Initialize the receive buffer so we can see data changes but also so we send
      predictable dummy bytes since we'll point to this buffer to source the transmit dummies */
      memset(SPI_Peripheral0->pu8RxBuffer, SPI_DUMMY_BYTE, SPI_Peripheral0->u16RxBufferSize);

      /* Load the PDC counter and pointer registers */
      SPI_Peripheral0->pBaseAddress->US_RPR = (unsigned int)SPI_Peripheral0->pu8RxBuffer; 
      SPI_Peripheral0->pBaseAddress->US_TPR = (unsigned int)SPI_Peripheral0->pu8RxBuffer; 
      SPI_Peripheral0->pBaseAddress->US_RCR = SPI_Peripheral0->u16RxBytes;
      SPI_Peripheral0->pBaseAddress->US_TCR = SPI_Peripheral0->u16RxBytes;

      /* When RCR is loaded, the ENDRX flag is cleared so it is safe to enable the interrupt */
      SPI_Peripheral0->pBaseAddress->US_IER = AT91C_US_ENDRX;
      
      /* Enable the receiver and transmitter to start the transfer */
      SPI_Peripheral0->pBaseAddress->US_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
      
    } /* End of receive function */
    else
    {
      /* Transmitting: update the message's status and flag that the peripheral is now busy */
      UpdateMessageStatus(SPI_Peripheral0->psTransmitBuffer->u32Token, SENDING);
      SPI_Peripheral0->u32PrivateFlags |= _SPI_PERIPHERAL_TX;    
      
      /* TRANSMIT SPI_SPI_SLAVE_FLOW_CONTROL */ 
      if(SPI_Peripheral0->eSpiMode == SPI_SLAVE_FLOW_CONTROL)
      {
        /* A Slave device with flow control uses interrupt-driven single byte transfers.
        CS must be asserted for the Slave to have queued data to get to here. */

        /* Load in the message parameters. */
        SPI_Peripheral0->u32CurrentTxBytesRemaining = SPI_Peripheral0->psTransmitBuffer->u32Size;
        SPI_Peripheral0->pu8CurrentTxData = SPI_Peripheral0->psTransmitBuffer->pu8Message;

        /* If we need LSB first, use inline assembly to flip bits with a single instruction. */
        u32Byte = 0x000000FF & *SPI_Peripheral0->pu8CurrentTxData;
        if(SPI_Peripheral0->eBitOrder == SPI_LSB_FIRST)
        {
          u32Byte = __RBIT(u32Byte)>>24;
        }
        
        /* This driver assumes half-duplex comms, so disable RX interrupt for now */
        SPI_Peripheral0->pBaseAddress->US_IDR = AT91C_US_RXRDY;
        
        /* Reset the transmitter since we have not been managing dummy bytes and it tends to be
        in the middle of a transmission or something that causes the wrong byte to get sent (at least on startup). */
        SPI_Peripheral0->pBaseAddress->US_CR = (AT91C_US_RSTTX);
        SPI_Peripheral0->pBaseAddress->US_CR = (AT91C_US_TXEN);
        SPI_Peripheral0->pBaseAddress->US_THR = (u8)u32Byte;
        SPI_Peripheral0->pBaseAddress->US_IER = AT91C_US_TXEMPTY;
        
        /* Trigger the callback which should provide flow-control to start transmitting */
        SPI_Peripheral0->fnSlaveTxFlowCallback();
      }
      
      /* TRANSMIT SPI_MASTER_AUTO_CS, SPI_MASTER_MANUAL_CS, SPI_SLAVE (no flow control) */
      /* A Master or Slave device without flow control uses the PDC */
      else
      {
        /* Load the PDC counter and pointer registers.  The "Next" pointers are never changed and will
        always point to SPI_u8Dummies with length 1.  */
        SPI_Peripheral0->pBaseAddress->US_TPR = (unsigned int)SPI_Peripheral0->psTransmitBuffer->pu8Message; 
        SPI_Peripheral0->pBaseAddress->US_TCR = SPI_Peripheral0->psTransmitBuffer->u32Size;
   
        /* When TCR is loaded, the ENDTX flag is cleared so it is safe to enable the interrupt */
        SPI_Peripheral0->pBaseAddress->US_IER = AT91C_US_ENDTX;
        
        /* Enable the transmitter to start the transfer */
        SPI_Peripheral0->pBaseAddress->US_PTCR = AT91C_PDC_TXTEN;
      }
    } /* End of transmitting function */
  }
  
} /* end SpiSM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void SpiSM_Error(void)          

@brief Handle an error 

*/
static void SpiSM_Error(void)          
{
  Spi_pfnStateMachine = SpiSM_Idle;
  
} /* end SpiSM_Error() */

        




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

