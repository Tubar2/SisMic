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
		mov 	#vetor, r5		;Initialize r5 with vetor address
		call	#menor
		jmp	$
		nop

menor:
		mov.b 	@r5+, r8		;Find word size
		mov.b 	#0xFF,r6		;Smallest Element
		clr		r7				;Frequency
loop:
		cmp.b	@r5+, r6		;Compare next word with current smallest
		jeq		equal			;Same word
		jhs		smaller			;New smaller word
exit:							;Exit loop condition
		dec.b	r8
		jnz		loop
		ret

smaller:						;Adapts registers to
		mov		#1, r7			;First appearence of word
		mov.b	-1(r5), r6		;New smallest word
		jmp		exit
		nop
equal:							;Adapts registers to
		inc		r7				;Another occurance of word
		jmp		exit
		nop


		.data
vetor:	.byte	13,"JOAQUIMJOSEA@"

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
            
