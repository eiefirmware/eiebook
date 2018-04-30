/*!**********************************************************************************************************************
@file sam3u_uart.h                                                                
@brief Header file for sam3u_uart.c
**********************************************************************************************************************/

#ifndef __SAM3U_UART_H
#define __SAM3U_UART_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

/*! 
@struct UartConfigurationType
@brief Task-provided parameters for a UART 
*/
typedef struct 
{
  PeripheralType UartPeripheral;      /* Easy name of peripheral */
  u16 u16RxBufferSize;                /* Size of receive buffer in bytes */
  u8* pu8RxBufferAddress;             /* Address to circular receive buffer */
  u8** pu8RxNextByte;                 /* Pointer to buffer location where next received byte will be placed */
  fnCode_type fnRxCallback;           /* Callback function for receiving data */
} UartConfigurationType;

/*! 
@struct UartPeripheralType
@brief Complete configuration parameters for a UART resource 
*/
typedef struct 
{
  AT91PS_USART pBaseAddress;          /* Base address of the associated peripheral */
  u32 u32PrivateFlags;                /* Flags for peripheral */
  MessageType* psTransmitBuffer;      /* Pointer to the transmit message linked list */
  u32 u32CurrentTxBytesRemaining;     /* Counter for bytes remaining in current transfer */
  u8* pu8CurrentTxData;               /* Pointer to current location in the Tx buffer */
  u8* pu8RxBuffer;                    /* Pointer to circular receive buffer in user application */
  u8** pu8RxNextByte;                 /* Pointer to buffer location where next received byte will be placed */
  fnCode_type fnRxCallback;           /* Callback function for receiving data */
  u16 u16RxBufferSize;                /* Size of receive buffer in bytes */
  u8 u8PeripheralId;                  /* Simple peripheral ID number */
  u8 u8Pad;
} UartPeripheralType;

/* u32PrivateFlags in UartPeripheralType */
#define   _UART_PERIPHERAL_ASSIGNED     (u32)0x00000001   /* Set when the peripheral is in use */
#define   _UART_PERIPHERAL_TX           (u32)0x00200000   /* Set when the peripheral is transmitting */
/* end u32PrivateFlags */


/**********************************************************************************************************************
* Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
UartPeripheralType* UartRequest(UartConfigurationType* psUartConfig_);
void UartRelease(UartPeripheralType* psUartPeripheral_);

u32 UartWriteByte(UartPeripheralType* psUartPeripheral_, u8 u8Byte_);
u32 UartWriteData(UartPeripheralType* psUartPeripheral_, u32 u32Size_, u8* pu8Data_);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UartInitialize(void);
void UartRunActiveState(void);

static void UartManualMode(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UartSM_Idle(void);
static void UartSM_Transmitting(void);
static void UartSM_Error(void);         


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/

/* Uart_u32Flags (local UART application flags) */
#define _UART_MANUAL_MODE               (u32)0x00000001   /* Set to push a transmit cycle during initialization mode */

#define _UART_NO_ACTIVE_UARTS           (u32)0x02000000   /* Set if Uart_u8ActiveUarts is 0 when decremented */
#define _UART_TOO_MANY_UARTS            (u32)0x04000000   /* Set if Uart_u8ActiveUarts is 0 when decremented */
/* end of Uart_u32Flags */




#endif /* __SAM3U_UART_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
