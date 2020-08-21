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
			mov		#vetor, r4		;Initialize r4
			call	#maior16
			jmp 	$
			nop

maior16:
			mov		@r4+,r8			;Vector size
			clr		r6				;Largest Element
			clr		r7				;Frequency
loop:
			cmp		@r4+,r6			;Compare new element to current largest
			jeq		equal			;Another word with same size as largest
			jlo		larger			;New larger word


exit:								;Exit condition
			dec		r8				;r8 == 0 => return
			jnz		loop			;else continue with loop
			ret

larger:
			mov		-1(r4), r6
			mov 	#1, r7
			jmp 	exit
			nop
equal:
			inc		r7
			jmp		exit
			nop

			.data
vetor:		.byte	6,0,"JOAQUIMJOSE",0

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
            
