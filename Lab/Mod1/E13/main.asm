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

;R6 -> i
;R7 -> j
;R8 -> LookedUp val 2
;R9 -> LookedUp val 1

main:
		mov 	#vetor, r6
		call 	#rom_arab
		jmp 	$
		nop

rom_arab:
		mov 	r6, r4			;Save the start of the vector on R4
		clr 	r5				;ANSWER
rewind:							;Rewind R6 to the end of roman vector
		inc 	r6
		cmp.b 	#0, 0(r6)
		jne 	rewind
algo:
		dec 	r6 ;Talvez aqui ou no loop
		cmp 	r6, r4
		jeq 	exit1
		call 	#lookUp
		mov 	r8, r9
		dec 	r6
		call 	#lookUp

		cmp 	r9, r8
		jhs 	rollback
exit2:
		add 	r9, r5
		cmp 	r6, r4
		jne 	algo
		ret
exit1:
		call 	#lookUp
		add 	r8, r5
		ret

rollback:
		inc 	r6
		jmp 	exit2


lookUp:
		cmp.b 	#'I', 0(r6)
		jeq 	caseI
		cmp.b 	#'V', 0(r6)
		jeq 	caseV
		cmp.b 	#'X', 0(r6)
		jeq 	caseX
		cmp.b 	#'L', 0(r6)
		jeq 	caseL
		cmp.b 	#'C', 0(r6)
		jeq 	caseC
		cmp.b 	#'D', 0(r6)
		jeq 	caseD
		cmp.b 	#'M', 0(r6)
		jeq 	caseM
caseI:
		mov 	#1, r8
		ret
caseV:
		mov 	#5, r8
		ret
caseX:
		mov 	#10, r8
		ret
caseL:
		mov 	#50, r8
		ret
caseC:
		mov 	#100, r8
		ret
caseD:
		mov 	#500, r8
		ret
caseM:
		mov 	#1000, r8
		ret


				.data
vetor: 			.byte 	"MMXIV", 0

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
            
