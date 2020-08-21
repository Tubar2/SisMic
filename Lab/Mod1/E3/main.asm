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
		mov		#vetor, r4	;Initialize vector in r4
		call	#m2m4
		jmp		$
		nop
m2m4:
		mov.b	@r4+, r8	;Size of vector
		clr		r6			;Multiples of 2
		clr 	r7			;Multiples of 4
loop:
		mov.b	@r4+, r5	;Get number from vector
							;Check if r5 elem is multiple of 2
		rra		r5
		jc		exit
		inc		r6
							;Check if multiple of 2 again => multiple of 4
		rra 	r5
		jc		exit
		inc		r7
exit:
		dec		r8
		jnz		loop
		ret


			.data
vetor:		.byte	8, 3, 5, 2, 8, 10, 20, 8, 1
                                            

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
            
