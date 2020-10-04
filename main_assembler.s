;##############################################################################
;# File: main_assembler.s
;##############################################################################

;------------------------------------------------------------------------------
; Description
; This is a demonstration program to show basic code structures in assembly
; A test comment.

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
				

main

move_eg MOV     r0, #10                 ;ro = d'10'
        MOV     r1, r0                  ;r1 = r0
        MOV     r0, r1, LSL #4          ;r0 = r1 *16 (bitshift 4 plekken naar links)
        MOV     r0, r1, ASR #1          ;r0 = signed(r1 / 2) (bitshift 1 plek naar rechts)
        MOV     r2, #8                  ;r2 = 8
        MOV     r0, r1, LSL r2          ;r0 = r1 * 2^r2 (LSL met register ipv getal)
        MOV     r0, r1, ASR r2          ;r0 = signed(r1 / 2^r2) (ASR met register ipv getal)
        MOV     r0, #0                  ;r0 = 0 (reset r0 to 0)
        MOV
        
	END