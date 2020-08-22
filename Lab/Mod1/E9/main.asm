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
		mov 	#1, r4
		mov 	#1, r5
		call 	#fib32
		jmp 	$
		nop
fib32:
		clr 	r10			;LSW
		clr 	r11			;MSW

		clr 	r6
		clr 	r7
loop:
		add 	r4, r5
		jc 		p2r4

		add 	r5,r4
		jc 	 	p2r5
		jmp 	loop
p2r4:
		adc 	r6
p2r4loop:
		add		r5,r4
		addc 	r6,r7
		jc		exitr5r6

		add 	r4,r5
		addc	r7,r6
		jc		exitr4r7
		jmp 	p2r4loop
		nop
p2r5:
		adc 	r6
p2r5loop:
		add		r4,r5
		addc 	r7,r6
		jc		exitr4r7

		add 	r5,r4
		addc 	r6,r7
		jc 		exitr5r6
		jmp 	p2r5loop
		nop
exitr5r6:
		mov 	r5, r10
		mov 	r6, r11
		ret
exitr4r7:
		mov 	r4, r10
		mov 	r7, r11
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
            
