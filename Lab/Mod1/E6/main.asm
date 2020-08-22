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
		mov 	#Vetor1, r4
		mov 	#Vetor2, r5
		call 	#sum_tot
		jmp 	$
		nop
sum_tot:
		mov 	@r4+, r6
		incd 	r5
		clr 	r7
		clr 	r8
loop:
		add 	@r4+, r7
		adc 	r8
		add 	@r5+, r7
		adc 	r8
exit:
		dec 	r6
		jnz 	loop
		ret

			.data

Vetor1: 	.word 7, 65000, 50054, 26472, 53000, 60606, 814, 41121
Vetor2: 	.word 7, 226, 3400, 26472, 470, 1020, 44444, 12345

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
            
