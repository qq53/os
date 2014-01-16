%include "sconst.inc"

NR_PRINTX				equ	0
NR_SENDREC				equ	1
INT_VECTOR_SYS_CALL		equ	0x90

global printx
global sendrec

BITS 32
[SECTION .text]

printx:
	mov eax,NR_PRINTX
	mov edx,[esp + 4]
	int INT_VECTOR_SYS_CALL
	ret

sendrec:
	mov eax,NR_SENDREC
	mov ebx,[esp + 4]
	mov ecx,[esp + 8]
	mov edx,[esp + 12]
	int INT_VECTOR_SYS_CALL
	ret
