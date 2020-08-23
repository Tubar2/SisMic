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
		mov 	#vetor, r4		;Pointer to vecotr (changes)
		call 	#ordena
		jmp 	$
		nop
ordena:
		mov.b 	@r4+, r7		;Find largest val length
		dec 	r7
		mov 	r7, r8			;Needed passes through vector
		mov 	r4, r5			;Cte to the beggining of vector
oLoop:
		mov 	r5, r4
		mov 	r4, r6			;Largest temp number
		mov 	r8, r7
iLoop:
		inc 	r4
		cmp.b 	0(r4), 0(r6)
		jlo		greater
exit_i:
		dec 	r7
		jnz 	iLoop
		jmp 	writeMem

greater:
		mov 	r4, r6
		jmp 	exit_i
writeMem:
		mov.b 	0(r6), r9
		mov.b 	0(r4), 0(r6)
		mov.b 	r9, 0(r4)
		dec 	r8
		jnz 	oLoop
		ret


			.data
vetor:		.byte 	5,4,7,3,5,1

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
            
