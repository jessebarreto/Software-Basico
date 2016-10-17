	.file	"calc_polones.c"
	.text
	.p2align 4,,15
	.globl	criar_elem_pilha
	.type	criar_elem_pilha, @function
criar_elem_pilha:
.LFB42:
	.cfi_startproc
	movq	%rbx, -16(%rsp)
	movq	%rbp, -8(%rsp)
	movq	%rdi, %rbx
	.cfi_offset 6, -16
	.cfi_offset 3, -24
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	$24, %edi
	movq	%rsi, %rbp
	call	malloc
	testq	%rax, %rax
	je	.L2
	movq	%rbx, (%rax)
	movl	$8, 8(%rax)
	movq	%rbp, 16(%rax)
.L2:
	movq	8(%rsp), %rbx
	movq	16(%rsp), %rbp
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE42:
	.size	criar_elem_pilha, .-criar_elem_pilha
	.p2align 4,,15
	.globl	push
	.type	push, @function
push:
.LFB43:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rsi, %rdi
	testq	%rbx, %rbx
	je	.L4
	movq	(%rbx), %rsi
	call	criar_elem_pilha
	movq	%rax, (%rbx)
.L4:
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE43:
	.size	push, .-push
	.p2align 4,,15
	.globl	pop
	.type	pop, @function
pop:
.LFB44:
	.cfi_startproc
	testq	%rdi, %rdi
	je	.L8
	movq	(%rdi), %rax
	testq	%rax, %rax
	je	.L9
	movq	16(%rax), %rdx
	movq	(%rax), %rax
	movq	%rdx, (%rdi)
	ret
	.p2align 4,,10
	.p2align 3
.L8:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L9:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE44:
	.size	pop, .-pop
	.p2align 4,,15
	.globl	criar_pilha
	.type	criar_pilha, @function
criar_pilha:
.LFB45:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movl	$8, %edi
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	call	malloc
	testq	%rax, %rax
	je	.L11
	testq	%rbx, %rbx
	je	.L12
	movq	%rax, %rdi
	movq	%rbx, %rsi
	movq	%rax, 8(%rsp)
	call	push
	movq	8(%rsp), %rax
.L11:
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L12:
	.cfi_restore_state
	movq	$0, (%rax)
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE45:
	.size	criar_pilha, .-criar_pilha
	.p2align 4,,15
	.globl	limpar_pilha
	.type	limpar_pilha, @function
limpar_pilha:
.LFB46:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	(%rdi), %rax
	movq	%rdi, %rbx
	testq	%rax, %rax
	je	.L14
	.p2align 4,,10
	.p2align 3
.L16:
	movq	16(%rax), %rdx
	movq	(%rax), %rdi
	movq	%rdx, (%rbx)
	call	free
	movq	(%rbx), %rax
	testq	%rax, %rax
	jne	.L16
.L14:
	movq	$0, (%rbx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE46:
	.size	limpar_pilha, .-limpar_pilha
	.p2align 4,,15
	.globl	checar_pilha
	.type	checar_pilha, @function
checar_pilha:
.LFB47:
	.cfi_startproc
	xorl	%eax, %eax
	cmpq	$0, (%rdi)
	setne	%al
	ret
	.cfi_endproc
.LFE47:
	.size	checar_pilha, .-checar_pilha
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"lvl:%d - %f\n"
	.text
	.p2align 4,,15
	.globl	mostrar_pilha_float
	.type	mostrar_pilha_float, @function
mostrar_pilha_float:
.LFB48:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	(%rdi), %rbx
	testq	%rbx, %rbx
	je	.L19
	movl	$1, %ebp
	.p2align 4,,10
	.p2align 3
.L21:
	movq	(%rbx), %rax
	movl	%ebp, %edx
	movl	$.LC0, %esi
	movl	$1, %edi
	addl	$1, %ebp
	movss	(%rax), %xmm0
	movl	$1, %eax
	cvtps2pd	%xmm0, %xmm0
	call	__printf_chk
	movq	16(%rbx), %rbx
	testq	%rbx, %rbx
	jne	.L21
.L19:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE48:
	.size	mostrar_pilha_float, .-mostrar_pilha_float
	.section	.rodata.str1.1
.LC1:
	.string	"Programa Finalizado."
	.text
	.p2align 4,,15
	.globl	sair_calc
	.type	sair_calc, @function
sair_calc:
.LFB51:
	.cfi_startproc
	movl	$.LC1, %edi
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	puts
	xorl	%edi, %edi
	call	exit
	.cfi_endproc
.LFE51:
	.size	sair_calc, .-sair_calc
	.section	.rodata.str1.1
.LC2:
	.string	"ERRO: %s\n"
	.text
	.p2align 4,,15
	.globl	erro_num
	.type	erro_num, @function
erro_num:
.LFB50:
	.cfi_startproc
	movq	%rdi, %rdx
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$.LC2, %esi
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk
	xorl	%eax, %eax
	call	sair_calc
	.cfi_endproc
.LFE50:
	.size	erro_num, .-erro_num
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"Operador %s n\303\243o reconhecido.\n"
	.align 8
.LC4:
	.string	"Operador Un\303\241rio %s Sem Operandos\n"
	.align 8
.LC5:
	.string	"Operador Bin\303\241rio %s Com Poucos Operandos\n"
	.text
	.p2align 4,,15
	.globl	erro_operador
	.type	erro_operador, @function
erro_operador:
.LFB49:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	testl	%edi, %edi
	je	.L29
	cmpl	$1, %edi
	je	.L30
	cmpl	$2, %edi
	je	.L31
.L27:
	xorl	%eax, %eax
	call	sair_calc
.L29:
	movq	%rsi, %rdx
	movb	$1, %dil
	movl	$.LC3, %esi
	xorl	%eax, %eax
	call	__printf_chk
	jmp	.L27
.L31:
	movq	%rsi, %rdx
	movb	$1, %dil
	movl	$.LC5, %esi
	xorl	%eax, %eax
	call	__printf_chk
	jmp	.L27
.L30:
	movq	%rsi, %rdx
	xorl	%eax, %eax
	movl	$.LC4, %esi
	call	__printf_chk
	jmp	.L27
	.cfi_endproc
.LFE49:
	.size	erro_operador, .-erro_operador
	.section	.rodata.str1.1
.LC6:
	.string	"Nada a ser feito!"
.LC9:
	.string	"IMPOSSIVEL DIVIDIR POR ZERO!"
.LC10:
	.string	"RESULTADO: %f\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB41:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rsi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movl	%edi, %ebx
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	cmpl	$1, %edi
	je	.L76
	xorl	%edi, %edi
	call	criar_pilha
	cmpl	$1, %ebx
	movq	%rax, %r12
	jle	.L34
	subl	$2, %ebx
	leaq	8(,%rbx,8), %r14
	xorl	%ebx, %ebx
	jmp	.L65
	.p2align 4,,10
	.p2align 3
.L78:
	cmpb	$48, %al
	jl	.L77
	movl	$4, %edi
	call	malloc
	xorl	%esi, %esi
	movq	%r15, %rdi
	movq	%rax, %rbp
	call	strtod
.L70:
	unpcklpd	%xmm0, %xmm0
	movq	%rbp, %rsi
	movq	%r12, %rdi
	cvtpd2ps	%xmm0, %xmm0
	movss	%xmm0, 0(%rbp)
	call	push
.L36:
	addq	$8, %rbx
	cmpq	%r14, %rbx
	je	.L34
.L65:
	movq	8(%r13,%rbx), %r15
	movzbl	(%r15), %eax
	cmpb	$57, %al
	jle	.L78
	cmpb	$112, %al
	je	.L44
	jg	.L48
	cmpb	$94, %al
	.p2align 4,,7
	je	.L42
	cmpb	$101, %al
	.p2align 4,,7
	jne	.L35
	cmpb	$0, 1(%r15)
	.p2align 4,,2
	je	.L79
.L49:
	movq	8(%r13,%rbx), %rsi
	cmpb	$120, 1(%rsi)
	jne	.L36
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L80
	movq	(%rax), %rbp
	movq	16(%rax), %rdx
	cvtss2sd	0(%rbp), %xmm0
	movq	%rdx, (%r12)
	call	exp
	jmp	.L70
	.p2align 4,,10
	.p2align 3
.L77:
	cmpb	$43, %al
	je	.L38
	.p2align 4,,9
	jle	.L81
	cmpb	$45, %al
	.p2align 4,,7
	je	.L39
	cmpb	$47, %al
	.p2align 4,,7
	jne	.L35
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L74
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	je	.L74
	movq	(%rax), %rbp
	movq	16(%rdx), %rax
	movq	%rax, (%r12)
	movq	(%rdx), %rax
	movss	(%rax), %xmm1
	cvttss2si	%xmm1, %eax
	testl	%eax, %eax
	je	.L75
	movss	0(%rbp), %xmm0
	movq	%rbp, %rsi
	divss	%xmm1, %xmm0
	movq	%r12, %rdi
	movss	%xmm0, 0(%rbp)
	call	push
	jmp	.L36
	.p2align 4,,10
	.p2align 3
.L48:
	cmpb	$120, %al
	je	.L82
.L35:
	movq	%r15, %rsi
	xorl	%edi, %edi
	call	erro_operador
.L81:
	testb	%al, %al
	je	.L36
	cmpb	$37, %al
	jne	.L35
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L74
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	je	.L74
	movq	(%rax), %rbp
	movq	16(%rdx), %rax
	movq	%rax, (%r12)
	movq	(%rdx), %rax
	cvttss2si	(%rax), %ecx
	testl	%ecx, %ecx
	je	.L75
	cvttss2si	0(%rbp), %eax
	movl	%eax, %edx
	movq	%rbp, %rsi
	sarl	$31, %edx
	movq	%r12, %rdi
	idivl	%ecx
	cvtsi2ss	%edx, %xmm0
	movss	%xmm0, 0(%rbp)
	call	push
	jmp	.L36
.L38:
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L74
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	je	.L74
	movq	(%rax), %rbp
	movq	16(%rdx), %rax
	movq	%r12, %rdi
	movss	0(%rbp), %xmm0
	movq	%rax, (%r12)
	movq	(%rdx), %rax
	movq	%rbp, %rsi
	addss	(%rax), %xmm0
	movss	%xmm0, 0(%rbp)
	call	push
	jmp	.L36
.L82:
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L74
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	je	.L74
	movq	(%rax), %rbp
	movq	16(%rdx), %rax
	movq	%r12, %rdi
	movss	0(%rbp), %xmm0
	movq	%rax, (%r12)
	movq	(%rdx), %rax
	movq	%rbp, %rsi
	mulss	(%rax), %xmm0
	movss	%xmm0, 0(%rbp)
	call	push
	jmp	.L36
.L39:
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L74
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	je	.L74
	movq	(%rax), %rbp
	movq	%rdx, (%r12)
	movq	%r12, %rdi
	movq	%rbp, %rsi
	call	push
	jmp	.L36
.L42:
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L74
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	je	.L74
	movq	(%rax), %rbp
	movq	16(%rdx), %rax
	movq	%rax, (%r12)
	movq	(%rdx), %rax
	cvtss2sd	0(%rbp), %xmm0
	cvtss2sd	(%rax), %xmm1
	call	pow
	jmp	.L70
.L44:
	movl	$4, %edi
	call	malloc
	movq	%r12, %rdi
	movl	$0x40490fd6, (%rax)
	movq	%rax, %rsi
	movq	%rax, %rbp
	call	push
	jmp	.L36
.L76:
	movl	$.LC6, %edi
	call	puts
	xorl	%edi, %edi
	call	exit
.L79:
	movl	$4, %edi
	call	malloc
	movq	%r12, %rdi
	movl	$0x402df854, (%rax)
	movq	%rax, %rsi
	movq	%rax, %rbp
	call	push
	jmp	.L49
.L74:
	movq	%r15, %rsi
	movl	$2, %edi
	call	erro_operador
.L34:
	xorl	%edx, %edx
	testq	%r12, %r12
	je	.L66
	movq	(%r12), %rax
	testq	%rax, %rax
	je	.L66
	movq	16(%rax), %rdx
	movq	%rdx, (%r12)
	movq	(%rax), %rdx
.L66:
	cvtss2sd	(%rdx), %xmm0
	movl	$.LC10, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movq	%r12, %rdi
	call	limpar_pilha
	movq	%rbp, %rdi
	call	free
	xorl	%eax, %eax
	call	sair_calc
.L75:
	movl	$.LC9, %edi
	call	erro_num
.L80:
	movl	$1, %edi
	call	erro_operador
	.cfi_endproc
.LFE41:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
