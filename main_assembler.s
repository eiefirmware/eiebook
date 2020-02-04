;##############################################################################
;# File: main_assembler.s
;##############################################################################

;------------------------------------------------------------------------------
; Description
; This is a demonstration program to show basic code structures in assembly
; A test comment.
; Neki komentar... kljdshflkdsjhgfaclgjkhfa
; Includes
#define  __ASSEMBLY__           1  ; Need this so the correct values in AT91SAM3U4.h are selected
#include "AT91SAM3U4.h"	           ; Include file for all special function registers
						                       ; and corresponding bit names
; A "MODULE" in an assembly file becomes and object file after it is assembled.
 		MODULE  main_assembler
    SECTION .text : CODE : NOROOT(2)
    THUMB

; "PUBLIC" declarations make labels from this file visible to others
		PUBLIC  main
;-----------------------------------------------------------------------
; Constants
#define PB_20_LED_RED 0x00100000
#define PB_19_LED_GRN 0x00080000
#define PB_18_LED_BLU 0x00040000
#define PB_17_LED_YLW 0x00020000
#define PB_16_LED_CYN 0x00010000
#define PB_15_LED_ORG 0x00008000
#define PB_14_LED_PRP 0x00004000
#define PB_13_LED_WHT 0x00002000

#define PB_02_BUTTON3 0x00000004

; Register address offsets
#define PER_OER_OFFSET (AT91C_PIOB_OER - AT91C_PIOB_PER)
#define PER_OWER_OFFSET (AT91C_PIOB_OWER - AT91C_PIOB_PER)
#define PER_SODR_OFFSET (AT91C_PIOB_SODR - AT91C_PIOB_PER)
#define PER_CODR_OFFSET (AT91C_PIOB_CODR - AT91C_PIOB_PER)
#define PER_ODSR_OFFSET (AT91C_PIOB_ODSR - AT91C_PIOB_PER)
#define PER_PDSR_OFFSET (AT91C_PIOB_PDSR - AT91C_PIOB_PER)
				

main

move_eg MOV r0, #0;
        MOV r1, #0;
        MOV r2, r1;
        MOVS r3, #0;
        ADD r0, r0, #256; r0=r0+256
        ADD r1, r1, r0, LSL #2; r1 = r1 + r0*4
        MOV r2, r0, LSR #6; r2 = r0/64
        SUBS r3, r1, r0; r3 = r1 - r0
        IT EQ
        MOVEQ r3, #1024;
        RSBS r3, r1, r0, LSL #4; r3 = 16*r0 - r1
        IT NE;
        MULNE r3, r2, r0; r3 = r2 * r0
        CMP r3, r1;
        IT EQ;
        ORREQS r3, r0, r1, LSR #10; r3 = r0 | r1/1024
        MOV r1, #0x20000000; r1 = target address
        MOV r0, #5000;
        STR r0,[r1]; 
        MOV r0, r0, LSL #4;
offset  MOV r3, #0x20; r3 = 32  -  adresni offset
        ADD r1, r1, r3; pomicem adresu za ofset
        STR r0, [r1];
        LDR r3, [r1];
        B main;
        
        

	END