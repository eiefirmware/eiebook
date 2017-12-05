;##############################################################################
;# File: main_assembler.s
;##############################################################################

;------------------------------------------------------------------------------
; Description
; This is a demonstration program to show basic code structures in assembly

; Includes
#define  __ASSEMBLY__           1  ; Need this so the correct values in AT91SAM3U4.h are selected
#include "AT91SAM3U4.h"	           ; Include file for all special function registers
						                       ; and corresponding bit names
; A "MODULE" in an assembly file becomes and object file after it is assembled.
 	MODULE  main_assembler
  SECTION .text : CODE : NOROOT(2)
  THUMB

; "EXTERN" declarations make labels from other files visible here
  EXTERN kill_x_cycles

; "PUBLIC" declarations make labels from this file visible to others
	PUBLIC  main
				
;------------------------------------------------------------------------------
; Constants
; These are mnemonics defined in the assembler space.  They are not yet defined
; in flash so you must use them correctly in the assembly code with instructions
; that can use the value directly from the immediate generator, or store them in
; flash and use load instructions to retrieve them.

#define PB_20_LED_RED     0x00100000
#define PB_19_LED_GRN     0x00080000
#define PB_18_LED_BLU     0x00040000
#define PB_17_LED_YLW     0x00020000
#define PB_16_LED_CYN     0x00010000
#define PB_15_LED_ORG     0x00008000
#define PB_14_LED_PRP     0x00004000
#define PB_13_LED_WHT     0x00002000

#define PB_02_BUTTON3     0x00000004

; Register address offsets 
#define PER_OER_OFFSET    (AT91C_PIOB_OER  - AT91C_PIOB_PER)
#define PER_OWER_OFFSET   (AT91C_PIOB_OWER - AT91C_PIOB_PER)
#define PER_SODR_OFFSET   (AT91C_PIOB_SODR - AT91C_PIOB_PER)
#define PER_CODR_OFFSET   (AT91C_PIOB_CODR - AT91C_PIOB_PER)
#define PER_ODSR_OFFSET   (AT91C_PIOB_ODSR - AT91C_PIOB_PER)
#define PER_PDSR_OFFSET   (AT91C_PIOB_PDSR - AT91C_PIOB_PER)

;--------------------------------------------------------------------------------------
; Power Management Controller Configuration
; PMC enable register init (set to turn on power to Port B)
#define PMC_PCER_INIT     (1 << AT91C_ID_PIOB)


;--------------------------------------------------------------------------------------
; Port B Configuration
; PIO enable register (set to make pin GPIO)
#define PIOB_PER_INIT     (PB_17_LED_YLW | PB_18_LED_BLU | PB_19_LED_GRN | PB_20_LED_RED | \
                           PB_16_LED_CYN | PB_15_LED_ORG | PB_14_LED_PRP | PB_13_LED_WHT | \
                           PB_02_BUTTON3)

; Output enable (data direction) registers (set to make pin an output)
#define PIOB_OER_INIT     (PB_17_LED_YLW | PB_18_LED_BLU | PB_19_LED_GRN | PB_20_LED_RED | \
                           PB_16_LED_CYN | PB_15_LED_ORG | PB_14_LED_PRP | PB_13_LED_WHT)

; Output write enable (set to allow output writes to ODSR)
#define PIOB_OWER_INIT    (PB_17_LED_YLW | PB_18_LED_BLU | PB_19_LED_GRN | PB_20_LED_RED)  

; Output set registers (set to make output pins low)
#define PIOB_CODR_INIT    0xFFFFFFFF  

; Watchdog timer control
#define WDTMR_INIT        0x00008000

;--------------------------------------------------------------------------------------
; Other constants
KILL_CYCLES       EQU     1000000     ; 250ms / (4e6 cycles/sec) = 1e6 cycles

;-----------------------------------------------------------------------------

main

; This code does nothing useful except illustrate some assembly langauge syntax
  MOV     r0, #10               ; r0 = 10
  MOV     r1, r0                ; r1 = r0
  MOV     r0, r1, LSL #4        ; r0 = r1 * 16
  MOV     r0, r1, ASR #1        ; r0 = signed(r1 / 2)
  
  MOV     r2, #8                ; r2 = 8
  MOV     r0, r1, LSL r2        ; r0 = r1 * 2^r2
  MOV     r0, r1, ASR r2        ; r0 = r1 / 2^r2 (signed)

; Longer example code from the notes
  MOV     r0, #0               ; r0 = 0
  MOV     r1, #0                ; r1 = 0
  MOV     r2, r1                ; r2 = r1
  
  MOVS    r3, #0                ; r3 = 0, update APSR

  ADD     r0, r0, #256          ; r0 = r0 + 256
  ADD     r1, r1, r0, LSL #2    ; r1 = r1 + (4 x r0)
  MOV     r2, r0, LSR #6        ; r2 = r0 / 64
  
  SUBS    r3, r1, r0            ; r3 = r1 - r0, update APSR
  
  IT      EQ
  MOVEQ   r3, #1024             ; if {Z}, r3 = 1024
  
  RSBS    r3, r1, r0, LSL #4    ; r3 = (16 x r0) - r1
  
  IT      NE
  MULNE   r3, r2, r0            ; r3 = r2 * r0
  
  CMP     r3, r1                ; Set flags if r1 == r3
  
  IT      EQ
  ORREQS  r3, r0, r1, LSR #10   ; r3 = r0 | (r1 / 1024)

; Load / store example code from the notes
  MOV     r1, #0x20000000       ; r1 = the target address
  
  LDR     r0, =5000             ; r0 = 5000
  
  STR     r0, [r1]              ; *r1 = r0
  
  LSL     r0, r0, #2            ; r0 *= 4
  ADD     r1, r1, #4            ; r1 += 4 (move the pointer)
  STR     r0, [r1]              ; *r1 = r0
  
  LDR     r3, [r1]              ; r3 = *r1


; Hello World program 
init
	LDR	    r0, =WDTMR_INIT                 ; Load r0 with the init constant
	LDR	    r1, =AT91C_WDTC_WDMR            ; Load r1 with address of the Watchdog Mode Register
	STR	    r0, [r1]                        ; *r1 = r0;   

	LDR	    r0, =PMC_PCER_INIT              ; Load r0 with the init constant
	LDR	    r1, =AT91C_PMC_PCER             ; Load r1 with address of PMC_PCER
	STR	    r0, [r1]                        ; *r1 = r0;   

	LDR	    r0, =PIOB_PER_INIT              ; Load R0 with the (location of the) constant
	LDR	    r2, =AT91C_PIOB_PER             ; Load R2 with address of PIOB_PER (a constant)
	STR	    r0, [r2]                        ; *r2 = r0; 
                                          
; From here, r2 will stay on AT91C_PIOB_PER so we do not have to waste instruction cycles
; to load it with different values.  Instead, we can just encode the offset into the 
; instructions we use to access the other PIOB registers.
	LDR	    r0, =PIOB_OER_INIT              ; Load R0 with the init constant
	STR	    r0, [r2, #PER_OER_OFFSET]       ; *(r2 + PER_OER_OFFSET) = r0
	STR	    r0, [r2, #PER_OWER_OFFSET]      ; *(r2 + PER_OWER_OFFSET) = r0
  LDR	    r0, =PIOB_CODR_INIT             ; Load R0 with the init constant
	STR	    r0, [r2, #PER_CODR_OFFSET]      ; *(r2 + PER_SODR_OFFSET) = r0
 
; r0 holds the number of cycles to call the delay function with
;	r2 still points to AT91C_PIOB_PER to read and write the port B GPIO
; r3 will be a working register for LEDs
; r4 will be a working register for buttons

; The main loop of the program calls the delay loop to kill 250ms worth of time
; This will toggle the BLU LED at 2Hz and give decent performance on the button response.
main_loop  
   
update_LED
	LDR		  r3, [r2, #PER_ODSR_OFFSET]      ; r3 = *(r2 + PER_ODSR_OFFSET) read the current AT91C_PIOB_ODSR value
  EOR     r3, r3, #PB_18_LED_BLU          ; r3 = r3 XOR the value to toggle the blue LED

check_button
 	LDR		  r4, [r2, #PER_PDSR_OFFSET]      ; r4 = *(r2 + PER_PDSR_OFFSET) read the current AT91C_PIOB_PDSR value
  ANDS    r4, r4, #PB_02_BUTTON3          ; Mask off all bits except BUTTON3
  BNE     button_not_pressed              ; if (BUTTON3)
  
button_pressed                            ; Add a breakpoint after this line
  ORR     r3, r3, #PB_17_LED_YLW          ; r3 |= PB_17_LED_YLW Turn on the yellow LED bit in r3
  B       continue                        ; Jump around the other case to continue the program
  
button_not_pressed
  BIC     r3, r3, #PB_17_LED_YLW          ; Clear the yellow LED bit and flow into "continue"
  
continue
  STR		  r3, [r2, #PER_ODSR_OFFSET]      ; Write the modified value back to port B

  LDR     r0, =KILL_CYCLES                ; r0 = cycles to kill (must be in r0 for the function call)
  BL      kill_x_cycles                   ; Call the function with a Branch and Link instruction (r14 loaded with return address)
	B	      main_loop			                  ; Repeat infinitely
	
	END