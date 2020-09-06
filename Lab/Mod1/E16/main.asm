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
delay1 		.equ 	500
delay2 		.equ 	500


main:
		clr 	r5
								;Default value for leds
		bic.b 	#BIT0, P1OUT
		bic.b 	#BIT7, P4OUT
								;Setting led bits pins as exit
		bis.b 	#BIT0, P1DIR
		bis.b 	#BIT7, P4DIR
							 	;Configuring pin as entry by setting its
		bic.b 	#BIT1, P1DIR 	;direction-bit to 0
								;Adding resistance to pin
		bis.b 	#BIT1, P1REN
								;Configuring resitance as a pull-down: button
		bis.b 	#BIT1, P1OUT	;realeased will read a 1 default value

loop:
		bit.b 	#BIT1, P1IN
		jc 		loop
		call 	#bin_led
		call 	#rot_atz
		jmp 	loop

;P4OUT => led1
;P1OUT => led2
bin_led:
		xor.b 	#BIT7, P4OUT
		jnz 	exit
		xor.b 	#BIT0, P1OUT
exit:
		ret

rot_atz:
		mov 	#delay1, r6
rt1:
		mov 	#delay2, r5
rt2:
		dec 	r5
		jnz 	rt2
		dec 	r6
		jnz 	rt1
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
            
