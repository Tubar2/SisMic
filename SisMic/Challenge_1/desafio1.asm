            .cdecls C,LIST,"msp430.h"       ; Include device header file
            .def    RESET                   ; Export program entry-point
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
		    mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
config:
			; configure aqui os pinos dos botões e LEDs
main:

			; Gera numero aleatorio
			; Espera um tempo aleatorio [128 ms a 4 sec]

			; Faz piscar os LEDs 1 vez

			; Aguarda o usuario responder

			; Mostra para o usuario se ele acertou ou nao
			; - Pode ter acertado
			; - Pode ter errado
			; - Acabou o tempo para responder

			; recomeça do inicio.

;-------------------------------------------------------------------------------
; Rotina debounce : faz a leitura dos botões e remove o rebote
; Saida
; - R12: leitura dos dois botões estaveis
readInput:
			; Complete aqui com o seu codigo
			ret
;-------------------------------------------------------------------------------
; Rotina display : Acende os LEDs com base no conteúdo de R12
; Entradas
; - R12: Número aleatorio gerado por lfsr
; Saidas
; - LEDs: Acende de acordo com BIT9 (Verde) e BIT1 (Vermelho)

display:
			; Complete aqui com o seu codigo
			ret

;-------------------------------------------------------------------------------
; Rotina wait : Espera um tempo dado em ms
; Entradas
; - R12: valor de tempo em ms
wait:
			; Complete aqui com o seu codigo
			ret

;-------------------------------------------------------------------------------
; Rotina lfsr: Linear Feedback Shift Register
; Saida
; - R12: Número pseudo-aleatorio
lfsr:

			; Complete com o seu codigo aqui
			ret
;-------------------------------------------------------------------------------
; RAM
;-------------------------------------------------------------------------------
			.data


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
            
