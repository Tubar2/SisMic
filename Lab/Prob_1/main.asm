;Ricardo de Castro Giometti Santos
;18/0027263
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
;Ricardo de Castro Giometti Santos
;18/0027263
Num 		.equ 	3999

main:
		mov 	#0x4400, SP			;Inicializing the stack
		mov 	#Num, r5			;Decimal number to be converted
		mov 	#Resp, r6			;Answer memory position
		call 	#alg_rom
		jmp 	$
		nop

alg_rom:
		push	r5
		push 	r6
									;Creating two arays:
		mov 	#nums, r12			;One for DECIMAL numbers
		mov 	#letters, r13		;One for ROMAN values

alg_rom_subtract:
		sub 	0(r12), r5
		jn 		alg_rom_negative
		cmp.b 	#0, 1(r13) 			;Checking if roman val is single or double
		jeq 	single_rom_insert

		mov.b 	0(r13), 0(r6)		;Double letter insertion
		mov.b 	1(r13), 1(r6)
		incd 	r6
		jmp 	alg_rom_subtract

single_rom_insert:					;Single letter insertion
		mov.b 	0(r13), 0(r6)
		inc 	r6
		jmp 	alg_rom_subtract

alg_rom_negative:
		add 	0(r12), r5
		incd 	r12
		incd 	r13

alg_rom_exit:
		cmp 	#0, r5
		jne 	alg_rom_subtract
		mov.b 	#0, 0(r6)
		pop 	r6
		pop 	r5
		ret

			.data
Resp: 		.byte	"RRRRRRRRRRRRRRRRRR",0
nums: 		.word 	1000,900,500,400,100,90,50,40,10,9,5,4,1
letters:	.byte 	"M",0, "CM", "D",0, "DC", "C",0, "XC", "L",0, "XL", "X",0, "IX", "V",0, "IV", "I",0
                                            

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
            
