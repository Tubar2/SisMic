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
		mov		#vetor1, r4		;Vector1 address on R4
		mov		#vetor2, r5		;Vector2 address on R5
		mov 	#0x2420, r7		;Solution Vector
		call 	#sum16
		jmp 	$
		nop
sum16:
		mov 	@r4+, r8
		incd	r5
loop:
		mov 	@r4+, 0(r7)		;Addition goes by moving first element from
								;vector1 to solution vector
		add		@r5+, 0(r7)		;Adds previuos moved element in solution with
								;element from vector2
		incd	r7
exit:
		dec 	r8
		jnz		loop
		ret



			.data
vetor1:		.word	7, 65000, 50054, 26472, 53000, 60606, 814, 41121
vetor2:		.word	7, 226, 3400, 26472, 470, 1020, 44444, 12345
                                            

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
            
