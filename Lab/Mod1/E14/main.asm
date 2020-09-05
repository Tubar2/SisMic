;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
DELAY1 		.equ 	450
DELAY2 		.equ 	300

main:
								;Starting both leds as off
		bic.b 	#BIT0, P1OUT
		bic.b 	#BIT7, P4OUT
								;Setting buffers so pins output signals
		bis.b 	#BIT0, P1DIR
		bis.b 	#BIT7, P4DIR
loop:
								;Turning red on for delay duration then off
		bis.b 	#BIT0, P1OUT
		call 	#rot_atz
		bic.b 	#BIT0, P1OUT
								;Turning green light on an off for duration
		bis.b 	#BIT7, P4OUT
		call 	#rot_atz
		bic.b 	#BIT7, P4OUT

		jmp 	loop

rot_atz:
		mov 	#DELAY1, r6
rt_1:
		mov 	#DELAY2, r5
rt_2:
		dec 	r5
		jnz 	rt_2
		dec 	r6
		jnz 	rt_1
		ret


;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
