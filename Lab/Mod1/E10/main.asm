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

cte 		.set	35243

main:
			mov 	#0x2400, r5 		;Memory pointer solution
			mov 	#cte, r6 			;Number to be converted
			call 	#w16_asc
			jmp 	$
			nop
w16_asc:
			mov 	#4, r7 				;One nible of  shifts
			clr 	r8					;Nibble TO BE converted
loopshift:
			rla 	r6					;Removing 4 bits, bit by bit
			rlc 	r8					;and concatenating to nibble
			dec 	r7
			jnz 	loopshift
			call	#nib_asc			;Convert nibble to ascii char
			mov 	r8, 0(r5) 			;Write ascii to memory
			incd 	r5					;Move to next memory position
			cmp 	#0x0000, r6			;Loop returns once all bits are shifted
			jnz 	w16_asc
			ret
nib_asc:
			cmp 	#10, r8				;Check if nibble is within [0-9]
			jlo 	menor
			jmp 	maior
menor:
			add 	#0x0030, r8
			ret
maior:
			add 	#0x0037, r8
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
            
