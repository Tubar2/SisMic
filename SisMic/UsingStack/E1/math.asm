		.def 		menor

;func menor() (menorEl, freq) : stack
;r4  : Endereço do vetor (bytes)
;r8  : tamanho do vetor
;r10 : Menor byte
;r11 : Frequencia
menor:
								;Salvar valores dos registradores usados
		push 	r4
		push 	r8
		push 	r10
		push 	r11

		mov.b 	@r4+, r8 		;Tamanho do vetor
		mov.b 	#0xFF, r10 		;Menor palavra
		clr 	r11 			;Frequencia

menor_loop:
		cmp.b 	@r4+, r10
		jeq 	menor_equal
		jhs 	menor_smaller

menor_exit:
		dec.b 	r8
		jnz 	menor_loop
								;Empurrar valores enontrado pela subrotina
		mov 	 r10, 10(SP)	;Salvar menor elemento
		mov 	 r11, 12(SP)	;Salvar frequência
								;Recuperar valores originais dos registradores
		pop 	r11
		pop 	r10
		pop 	r8
		pop 	r4
		ret

menor_equal:
		inc 	r11
		jmp 	menor_exit
		nop

menor_smaller:
		mov 	#1, r11
		mov.b 	-1(r4), r10
		jmp		menor_exit
		nop
