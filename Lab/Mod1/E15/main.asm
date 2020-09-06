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
loop:
		bit.b 	#BIT1, P2IN
		jc 		btn1_notpress
		bis.b 	#BIT0, P1OUT	;BtnPress: ativar led == enviar sinal para o
loop2:								;bit 0 do pino 1
		bit.b 	#BIT1, P1IN
		jc 		btn2_notpress
		bis.b 	#BIT7, P4OUT	;BtnPress: ativar led == enviar sinal para o
								;bit 0 do pino 1
		jmp 	loop

btn1_notpress:
		bic.b 	#BIT0, P1OUT
		jmp 	loop2
btn2_notpress:
		bic.b 	#BIT7, P4OUT
		jmp 	loop
		nop

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
            
