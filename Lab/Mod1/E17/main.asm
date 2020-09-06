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
		bic.b 	#BIT0, P1OUT
		bic.b 	#BIT7, P4OUT

		bis.b 	#BIT0, P1DIR
		bis.b 	#BIT7, P4DIR

		bic.b 	#BIT1, P2DIR 	;Configuring both pins as entry by setting its
		bic.b 	#BIT1, P1DIR 	;direction-bit to 0

		bis.b 	#BIT1, P2REN	;Adding resistance to both pins
		bis.b 	#BIT1, P1REN

		bis.b 	#BIT1, P2OUT	;Configuring resitance as a pull-down: button
		bis.b 	#BIT1, P1OUT	;realeased will read a 1 default value

AA:
		clr 	r7
		bic.b 	#BIT0, P1OUT
		bic.b 	#BIT7, P4OUT
		call 	#rot_atz
loop:
		bit.b 	#BIT1, P2IN
		jnc 	FA
		bit.b 	#BIT1, P1IN
		jnc 	AF
		jmp 	loop

FA:
		bis.b 	#BIT0, P1OUT
		call 	#rot_atz
fa_loop:
		bit.b 	#BIT1, P2IN
		jnc 	AA
		bit.b 	#BIT1, P1IN
		jnc 	FF
		jmp 	fa_loop

AF:
		bis.b 	#BIT7, P4OUT
		call 	#rot_atz
af_loop:
		bit.b 	#BIT1, P2IN
		jnc 	FF
		bit.b 	#BIT1, P1IN
		jnc 	AA
		jmp 	af_loop
FF:
		xor.b 	#BIT0, P1OUT
		xor.b 	#BIT7, P4OUT
		call 	#rot_atz
		xor.b 	#BIT0, P1OUT
		xor.b 	#BIT7, P4OUT
		call 	#rot_atz
ff_exit1:
		bit.b 	#BIT1, P2IN
		jnc 	p1_ret
ff_exit2:
		bit.b 	#BIT1, P1IN
		jnc 	p2_ret
ff_exit:
		cmp.b 	#3, r7
		jeq 	AA
		jmp 	FF

p1_ret:
		bis.b 	#BIT0,r7
		jmp 	ff_exit2
p2_ret:
		bis.b 	#BIT1,r7
		jmp 	ff_exit

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
            
