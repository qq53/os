[SECTION .text]

global memcpy
global memset
global strcpy
global strlen

memcpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
	test ecx,ecx
	jz .2
	cld
.1:
	;lodsb
	;stosb
	mov al,[ds:esi]
	inc esi

	mov [es:edi],al
	inc edi

	loop .1
	
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回

memset:
	push ebp
	mov ebp,esp

	push esi
	push edi
	push ecx

	mov edi,[ebp+8]
	mov edx,[ebp+12]
	mov ecx,[ebp+16]
	test ecx,ecx
	jz .2
.1:
	mov byte [edi],dl
	inc edi

	loop .1
.2:
	pop ecx
	pop edi
	pop esi
	mov esp,ebp
	pop ebp

	ret

strcpy:
	push ebp
	mov ebp,esp

	mov esi,[ebp+12]
	mov edi,[ebp+8]
	cld

.1:
	lodsb
	stosb
	test al,al
	jnz .1

	mov eax,[ebp+8]

	pop ebp
	ret

strlen:
	push ebp
	mov ebp,esp

	xor eax,eax
	mov esi,[ebp+8]

.1:
	cmp byte [esi],0
	jz .2
	inc esi
	inc eax
	jmp .1

.2:
	pop ebp
	ret
