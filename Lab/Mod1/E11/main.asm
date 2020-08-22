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
			mov 	#vetor, r4
			clr 	r6
			call 	#asc_w16
ok:			jc 		ok
nok:		jnc 	nok
asc_w16:
			mov 	#4, r9
asc_loop:
			mov.b 	@r4+, r5		;Get nibble from memory
			cmp.b 	#0x30, r5
			jlo 	badcase			; nib < h30 (Badcase)
			cmp.b 	#0x39, r5
			jlo		lowerBound 		; h30 <= nib <= h39 (Goodcase)
			cmp.b 	#0x41, r5
			jlo 	badcase			; h3A < nib < h40 (Badcase)
			cmp.b 	#0x47, r5
			jlo 	upperBound 		; h41 <= nib <= h46 (Goodcase)
			jhs 	badcase 		; nib >= h47 (Badcase)
exit:
			dec 	r9
			jnz 	asc_loop
			setc
			ret

lowerBound:
			add.b 	#-0x30, r5 		;nib = nib -30
			call 	#concatR6
			jmp 	exit
upperBound:
			add.b 	#-0x37, r5 		;nib = nib - 37
			call 	#concatR6
			jmp 	exit
badcase:
			ret

;concatR6(R5, R6):
	;concatR6 will append last 4 bits of R5 to R6
	;as it's proper value, by reversing R5 before
	;appending
	;R5: 0000-0000-0000-1000 -> 1000-0000-0000-0000
concatR6:
			mov 	#4, r8
			mov 	#4, r7
			clrc
invertR5:
			rrc 	r5
			rrc 	r5
			rrc 	r5
			rrc 	r5
			rrc 	r5
appendR6:
			rla 	r5
			rlc 	r6
			dec 	r8
			jnz 	appendR6
			ret


			.data
;Vector of ascii character nibbles (4 bits)
vetor:		.byte	'8', ';', 'A', 'B'

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
            
