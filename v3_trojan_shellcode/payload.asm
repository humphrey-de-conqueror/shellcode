

BITS 64 

section .data 
msg:		db	"penguin love eating french fries", 0x0A 
msg_len:	equ	$ - msg 


section .text
global _start

_start: 
	mov rax, 1
	mov rdi, 1
	lea rsi, [rel msg]
	mov rdx, msg_len
	syscall 

	mov rax, 60 
	xor rdi, rdi 
	syscall 