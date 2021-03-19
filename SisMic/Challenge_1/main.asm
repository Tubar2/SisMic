            .cdecls C,LIST,"msp430.h"       ; Include device header file
            .def    RESET                   ; Export program entry-point
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
		    mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
; Constantes auxiliares
BIT11:		.equ	0x800
BIT10:	 	.equ 	0x400

; Delays para funções de atraso
dispDelay: 	.equ 	5000
mainDelay:	.equ 	50000

config:
			; configure aqui os pinos dos botıes e LEDs
		bic.b 	#BIT0, P1OUT	;Configurando entrada inical 0 para os Leds
		bic.b 	#BIT7, P4OUT

		bis.b 	#BIT0, P1DIR	;Configurando pinos 1.0: Esquerdo : Vermelho
		bis.b 	#BIT7, P4DIR	;e 4.7 Direito : Verde como saída

		bic.b 	#BIT1, P2DIR 	;Configurando pinos 2.1: BTN Esquerdo
		bic.b 	#BIT1, P1DIR	;e 1.1: BTN Direito como entrada

		bis.b 	#BIT1, P2REN	;Configurnado resistência
		bis.b 	#BIT1, P1REN

		bis.b 	#BIT1, P2OUT	;Registrando resistor de pull-up
		bis.b 	#BIT1, P1OUT

	;Numero inicial para lfsr (seed)
	mov 	#0x0121, r12
								;Configurando tempo máximo de espera
	mov 	#0xA000, r10		;LSB time
	mov 	#0x0000, r11		;MSB time

main:
		; R8 -> Representa o(s) led(s) apertado(s). É entrada de display
		; 	- 01: Apenas led direito
		;   - 10: Apenas led esquerdo
		;   - 11: Ambos os leds
		clr 	r8
								; Gera numero aleatorio
		call 	#lfsr
								; Espera um tempo aleatorio [128 ms a 4 sec]
		call 	#wait
								; Faz piscar os LEDs 1 vez
		call 	#display
								; Aguarda o usuario responder
		call 	#readInput
								; Mostra para o usuario se ele acertou ou nao
								; - Pode ter acertado
								; - Pode ter errado
								; - Acabou o tempo para responder
		mov 	#6, r9
		cmp 	r5, r11			; Checando por tempo excedido com MSB e LSB
		jlo 	tempo_esgotado	; R5 > R11 -> Tempo esgotado automaticamente
		cmp 	r4, r10			; Tempo esgotado em LSB caso R11 == 0
		jlo 	tempo_esgotado1	; R4 > R10 -> Possível tempo esgotado (depende de r11)
haTempo:
		cmp 	r6, r8			; Comparando entrada do usuária com a saída dos led
		jeq 	acertou
errou:							; Em caso de erro, o led vermelho pisca 3 vezes
		xor.b	#BIT0, P1OUT
		call 	#main_delay
		dec 	r9
		jnz 	errou
		call 	#main_delay
								; recomeÁa do inicio.
		jmp 	main

acertou:						; Em caso de acerto, o led verde pisca 3 vezes
		xor.b	#BIT7, P4OUT
		call 	#main_delay
		dec 	r9
		jnz 	acertou
		call 	#main_delay
								; recomeÁa do inicio.
		jmp 	main

tempo_esgotado1:				; Checando por tempo excedido em LSB
		cmp 	r5, r11
		jne 	haTempo
tempo_esgotado:					; Tempo excedido em MSB + LSB
		xor.b	#BIT0, P1OUT	; pisca led vermelhor + verde 3 vezes
		xor.b 	#BIT7, P4OUT
		call 	#main_delay
		dec 	r9
		jnz 	tempo_esgotado
		call 	#main_delay
								; recomeÁa do inicio.
		jmp 	main

; Delay para piscar o led 3 vezes nos casos de acerto, erro ou tempo  esgotado
main_delay:
		push 	r10
		mov 	#mainDelay, r10
main_delay_loop:
		dec  	r10
		jnz 	main_delay_loop

		pop 	r10
		ret

;-------------------------------------------------------------------------------
; Rotina debounce : faz a leitura dos botıes e remove o rebote
; Saida
; - R6: leitura dos dois botıes estaveis
readInput:
		; R4 + R5 = Timer
		clr 	r4;	LSB
		clr 	r5; MSB
		clr 	r6

amostrar:
							;Primeira parte: checar por um clique qualquer
		call 	#temporizador
		bit.b	#BIT1, P2IN
		jnc		PA			;BTN esquerdo press
		bit.b	#BIT1, P1IN
		jnc 	AP			;BTN direito press
		jmp 	amostrar
AP:
		bis.b 	#BIT0, r6
AP_loop:
		bit.b	#BIT1, P1IN
		jc 		exit 		;AP -> AA (soltar botão)
		bit.b	#BIT1, P2IN
		jnc		PP			;AP -> PP (pressionar botão direito)
		call 	#temporizador
		jmp 	AP_loop
PA:
		bis.b 	#BIT1, r6
PA_loop:
		bit.b 	#BIT1, P2IN
		jc 		exit 		;PA -> AA (soltar botão)
		bit.b 	#BIT1, P1IN
		jnc 	PP 			;AP -> PP (pressionar botão esquerdo)
		call 	#temporizador
		jmp 	PA_loop
PP:
		bis.b 	#BIT0, r6
		bis.b 	#BIT1, r6
PP_loop:
		bit.b	#BIT1, P2IN
		jc		AP_loop			;Caso soltar apenas um botão (esquerdo)
		bit.b 	#BIT1, P1IN
		jc 		PA_loop			;Caso soltar apenas um botão (direito)
		call 	#temporizador
		jmp 	PP_loop

exit:
			ret

; Rotina que incrementa o tempo que o usuário leva para
; completar apertar e soltar um ou mais botões
temporizador:
		inc 	r4
		adc 	r5

		ret
;-------------------------------------------------------------------------------
; Rotina display : Acende os LEDs com base no conte˙do de R12
; Entradas
; - R12: N˙mero aleatorio gerado por lfsr
; Saidas
; - LEDs: Acende de acordo com BIT9 (Verde) e BIT1 (Vermelho)
; - R8: Leds que acenderam

display:
		bit.b 	#BIT1, r12		;Test bit1 de r12
		jc 		blinkB1			;c==1 then ligar led1
display2:
		bit 	#0x200, r12		;Test bit 9 de r12
		jc 		blinkB2			;c==1 then ligar led2
								;Caso nenhum bit acenda
		cmp 	#0, r8			;Nova chamada para lfsr
		jne 	exit_display	;e recalcular r12
		call 	#lfsr
		jmp 	display

; Acende led1 e configura R8 para representar que o led1 foi aceso
blinkB1:
		bis.b 	#BIT1, r8
		bis.b 	#BIT0, P1OUT
		jmp 	display2
; Acende led2 e configura R8 para representar que o led2 foi aceso
blinkB2:
		bis.b 	#BIT0, r8
		bis.b 	#BIT7, P4OUT
exit_display:
		call 	#display_delay
		bic.b 	#BIT0, P1OUT
		bic.b 	#BIT7, P4OUT
		ret

; Delay para o blink dos LEDs
display_delay:
		push 	r4
		mov 	#dispDelay, r4
display_delay_loop:
		dec 	r4
		jnz 	display_delay_loop

		pop 	r4
		ret

;-------------------------------------------------------------------------------
; Rotina wait : Espera um tempo dado em ms
; Entradas
; - R12: valor de tempo em ms
wait:
								; Complete aqui com o seu codigo
		push 	r13
		push 	r14

		mov 	#256, r14
wait_loop:
		mov 	r12, r13
wait_loop2:
		dec 	r13
		jnz 	wait_loop2
		dec		r14
		jnz 	wait_loop

		pop 	r14
		pop 	r13
		ret

;-------------------------------------------------------------------------------
; Rotina lfsr: Linear Feedback Shift Register
; Entrada
; - R12: Numero anterior
; Saida
; - R12: N˙mero pseudo-aleatorio
lfsr:
		rrc 	r12				; shift para a direita
		jc		adaptR12		; checa caso o prévio bit0 era 1
		bic 	#BIT11, r12		; caso não, o bit11 será 0
lfsr2:
		bic 	#0xF000, r12;	; zerar últimos 4 bits

		ret
; Adapta r12 para que caso o bit0 fosse 1, haverá o xor
adaptR12:
		bis 	#BIT11, r12
		xor 	#(BIT10|BIT7|BIT5), r12
		jmp 	lfsr2
		nop

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
            
