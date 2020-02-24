	.file	"main.c"
	.text
	.section	.rodata
	.align 8
.LC0:
	.string	"\033[31mUsage: gba \"CART_PATH\"\n\033[0m"
	.text
	.globl	usage
	.type	usage, @function
usage:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1, %edi
	call	exit@PLT
	.cfi_endproc
.LFE6:
	.size	usage, .-usage
	.section	.rodata
.LC1:
	.string	"r"
	.align 8
.LC2:
	.string	"Couldn't load my cool title graphic :("
.LC3:
	.string	"\033[36m%c\033[0m"
	.text
	.globl	printCard
	.type	printCard, @function
printCard:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	leaq	.LC1(%rip), %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L3
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	jmp	.L2
.L3:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	fgetc@PLT
	movb	%al, -9(%rbp)
	jmp	.L5
.L6:
	movsbl	-9(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	fgetc@PLT
	movb	%al, -9(%rbp)
.L5:
	cmpb	$-1, -9(%rbp)
	jne	.L6
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
.L2:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	printCard, .-printCard
	.section	.rodata
.LC4:
	.string	"assets/title"
.LC5:
	.string	"assets/exit"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	cmpl	$1, -68(%rbp)
	jg	.L8
	movl	$0, %eax
	call	usage
.L8:
	movq	-80(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -48(%rbp)
	leaq	.LC4(%rip), %rdi
	call	printCard
	movl	$0, -56(%rbp)
	leaq	-56(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	loadCart@PLT
	movq	%rax, -40(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	initCPU@PLT
	movw	$0, -22(%rbp)
	movw	$0, -24(%rbp)
	leaq	-32(%rbp), %rax
	movl	$3, %esi
	movq	%rax, %rdi
	call	getRPRegister@PLT
	movl	$193, %esi
	movq	%rax, %rdi
	call	LD16@PLT
	leaq	-32(%rbp), %rax
	movl	$7, %esi
	movq	%rax, %rdi
	call	getRegister@PLT
	movl	$171, %esi
	movq	%rax, %rdi
	call	LD@PLT
	movl	$0, -52(%rbp)
	jmp	.L9
.L10:
	subq	$8, %rsp
	pushq	-16(%rbp)
	pushq	-24(%rbp)
	pushq	-32(%rbp)
	call	printDebug@PLT
	addq	$32, %rsp
	movq	-40(%rbp), %rdx
	leaq	-32(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	run_cycle@PLT
	addl	$1, -52(%rbp)
.L9:
	cmpl	$4, -52(%rbp)
	jle	.L10
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	freeCPU@PLT
	leaq	.LC5(%rip), %rdi
	call	printCard
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L12
	call	__stack_chk_fail@PLT
.L12:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.ident	"GCC: (Arch Linux 9.2.1+20200130-2) 9.2.1 20200130"
	.section	.note.GNU-stack,"",@progbits
