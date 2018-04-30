/*!*********************************************************************************************************************
@file configuration.h      
@brief Main configuration header file for project.  

This file bridges many of the generic features of the firmware to the 
specific features of the design. The definitions should be updated
to match the target hardware.  
 
Bookmarks:
!!!!! External module peripheral assignments
##### Communication peripheral board-specific parameters


***********************************************************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

/**********************************************************************************************************************
Runtime switches
***********************************************************************************************************************/
//#define MPGL2_R01                   /*!< Use with MPGL2-EHDW-01 revision board */


/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

/*! 
@enum PeripheralType
@brief Short names used to identify peripherals in their configuration structs.

This provides self-documentation when indexing peripherals and when assigning
better names for devices that make use of the peripherals.
*/
typedef enum {SPI, UART, USART0, USART1, USART2, USART3} PeripheralType;


/**********************************************************************************************************************
Includes
***********************************************************************************************************************/
/* Common header files */
#include <stdlib.h>
#include <string.h>
#include "AT91SAM3U4.h"
#include "exceptions.h"
#include "interrupts.h"
#include "core_cm3.h"
#include "main.h"
#include "typedefs.h"
#include "utilities.h"

/* EIEF1-PCB-01 specific header files */
#ifdef EIE1
#include "eief1-pcb-01.h"
#endif /* EIE1 */

#ifdef MPGL2
/* MPGL2-specific header files */
#ifdef MPGL2_R01
#include "mpgl2-ehdw-01.h"
#else
#include "mpgl2-ehdw-02.h"
#endif /* MPGL2_R01 */
#endif /* MPGL2 */

/* Common driver header files */
#include "buttons.h"
#include "leds.h" 
#include "messaging.h"
#include "timer.h"

#include "sam3u_uart.h"

/* Common application header files */
#include "debug.h"
#include "music.h"
#include "user_app1.h"
#include "user_app2.h"
#include "user_app3.h"


/**********************************************************************************************************************
!!!!! External device peripheral assignments
***********************************************************************************************************************/

/* Peripheral assignments */
#define BLADE_UART                  UART
#define DEBUG_UART                  USART0
#define ANT_SPI                     USART2
#define BLADE_SPI                   SPI

#ifdef EIE1
#define SD_SSP                      USART1
#endif

#ifdef MPGL2
#define LCD_SPI                     USART1
#endif


/*! @cond DOXYGEN_EXCLUDE */
/* %UART% Configuration */

/* Debug UART Peripheral Allocation (USART0) */
#define USART0_US_CR_INIT           DEBUG_US_CR_INIT
#define USART0_US_MR_INIT           DEBUG_US_MR_INIT
#define USART0_US_IER_INIT          DEBUG_US_IER_INIT
#define USART0_US_IDR_INIT          DEBUG_US_IDR_INIT
#define USART0_US_BRGR_INIT         DEBUG_US_BRGR_INIT

#define UART0_IRQHandler            USART0_IrqHandler
#define DEBUG_UART_PERIPHERAL       AT91C_ID_US0

/* Blade UART Peripheral Allocation (UART) */
#define UART_US_CR_INIT             BLADE_US_CR_INIT
#define UART_US_MR_INIT             BLADE_US_MR_INIT
#define UART_US_IER_INIT            BLADE_US_IER_INIT
#define UART_US_IDR_INIT            BLADE_US_IDR_INIT
#define UART_US_BRGR_INIT           BLADE_US_BRGR_INIT

#define UART_IRQHandler             DBGU_IrqHandler
#define BLADE_UART_PERIPHERAL       AT91C_ID_DBGU


/***********************************************************************************************************************
##### Communication peripheral board-specific parameters
***********************************************************************************************************************/

/*! @cond DOXYGEN_EXCLUDE */
/*----------------------------------------------------------------------------------------------------------------------
%UART%  Configuration                                                                                                  
----------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Debug UART Setup

Debug is used for the terminal (serial: 115.2k, 8-N-1) debugging interface.
*/
/* USART Control Register */
#define DEBUG_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register */
#define DEBUG_US_MR_INIT (u32)0x000008C0
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [0] VAR_SYNC sync field 
    21 [0] DSNACK delicious! NACK is sent on ISO line immediately on parity error
    20 [0] INACK NACK generated (N/A for async)

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [0] MSBF/CPOL LSB first

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP 1 stop bit
    12 [0] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] SYNC/CPHA asynchronous

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [0] USART_MODE normal
    02 [0] "
    01 [0] "
    00 [0] "
*/


/* USART Interrupt Enable Register */
#define DEBUG_US_IER_INIT (u32)0x00000008
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled yet

    03 [1] ENDRX End of Receiver Transfer (PDC) interrupt enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt enabled
*/

/* USART Interrupt Disable Register */
#define DEBUG_US_IDR_INIT (u32)~DEBUG_US_IER_INIT

/* USART Baud Rate Generator Register 
BAUD = MCK / (8(2-OVER)(CD + FP / 8))
=> CD = (MCK / (8(2-OVER)BAUD)) - (FP / 8)
MCK = 48MHz
OVER = 0 (16-bit oversampling)

BAUD desired = 38400 bps
=> CD = 78.125 - (FP / 8)
Set FP = 1, CD = 78 = 0x4E

BAUD desired = 115200 bps
=> CD = 26.042 - (FP / 8)
Set FP = 0, CD = 26 = 0x1A

*/
#define DEBUG_US_BRGR_INIT (u32)0x0000001A
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP = 0
    17 [0] "
    16 [0] "

    15 [0] CD = 26 = 0x1A
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [0] "
    01 [1] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
Blade UART Setup

The Blade UART is used for the daughter board interface (serial: 115.2k, 8-N-1) .
*/
/* USART Control Register */
#define BLADE_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register */
#define BLADE_US_MR_INIT (u32)0x000008C0
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [0] VAR_SYNC sync field 
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK NACK generated (N/A for async)

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [0] MSBF/CPOL LSB first

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP 1 stop bit
    12 [0] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] SYNC/CPHA asynchronous

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [0] USART_MODE normal
    02 [0] "
    01 [0] "
    00 [0] "
*/


/* USART Interrupt Enable Register */
#define BLADE_US_IER_INIT (u32)0x00000008
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled yet

    03 [1] ENDRX End of Receiver Transfer (PDC) interrupt enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt not enabled
*/

/* USART Interrupt Disable Register */
#define BLADE_US_IDR_INIT (u32)~BLADE_US_IER_INIT

/* USART Baud Rate Generator Register
BAUD = MCK / (8(2-OVER)(CD + FP / 8))
=> CD = (MCK / (8(2-OVER)BAUD)) - (FP / 8)
MCK = 48MHz
OVER = 0 (16-bit oversampling)

BAUD desired = 115200 bps
=> CD = 26.042 - (FP / 8)
Set FP = 0, CD = 26

*/
#define BLADE_US_BRGR_INIT (u32)0x0000001A
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP = 0
    17 [0] "
    16 [0] "

    15 [0] CD = 26 = 0x1A
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [0] "
    01 [1] "
    00 [0] "
*/


/*! @endcond */






#endif /* __CONFIG_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

