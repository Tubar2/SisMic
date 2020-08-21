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
			mov		#vetor, r4
			call	#extremos
			jmp		$
			nop
extremos:
			mov		@r4+, r8		;Vector size
			mov		@r4, r6			;Smallest element
			mov		@r4+, r7		;Largest element
			dec		r8
loop:
			mov 	@r4+, r5		;Element to be compared
			cmp		r5, r7			;Compare new element with current largest
			jl		larger
			cmp 	r5, r6			;Compare new element with current smallest
			jge		smaller
exit:
			dec		r8
			jnz		loop
			ret
larger:
			mov		r5, r7
			jmp		exit
smaller:
			mov		r5, r6
			jmp		exit
			nop


			.data
vetor:		.word	8, 121, 234, 567, -1990, 117, 867, 45, -1980

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
            
