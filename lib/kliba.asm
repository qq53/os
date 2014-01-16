%include "sconst.inc"

[SECTION .text]

global disp_str
global out_byte
global in_byte
global disp_int
global disp_color_str
global disable_irq
global enable_irq
global disable_int
global enable_int

;全局变量
extern disp_pos

disp_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[disp_pos], edi

	pop	ebp
	ret

out_byte:
	mov edx,[esp + 4]
	mov al,[esp + 8]
	out dx,al
	nop
	nop
	ret

in_byte:
	mov edx,[esp + 4]
	xor eax,eax
	in al,dx
	nop
	nop
	ret

disp_color_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, [ebp + 12]
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi,2
	jmp	.1

.2:
	mov	[disp_pos], edi

	pop	ebp
	ret

;enable_irq(int)
enable_irq:
	mov ecx,[esp + 4]
	pushf			;保护标志寄存器
	cli
	mov ah,~1
	rol ah,cl
	cmp cl,8
	jae .8

	in al,INT_M_CTLMASK
	and al,ah
	out INT_M_CTLMASK,al
	popf
	ret
.8:
	in al,INT_S_CTLMASK
	and al,ah
	out INT_S_CTLMASK,al
	popf
	ret

;disable_irq(int)
disable_irq:
	mov ecx,[esp + 4]
	pushf
	cli
	mov ah,1
	rol ah,cl
	cmp cl,8
	jae .8
	
	in al,INT_M_CTLMASK
	test al,ah
	jnz .already
	or al,ah
	out INT_M_CTLMASK,al
	popf
	mov eax,1
	ret
.8:
	in al,INT_S_CTLMASK
	test al,ah
	jnz .already
	or al,ah
	out INT_S_CTLMASK,al
	popf
	mov eax,1
	ret
.already:
	xor eax,eax
	popf
	ret

enable_int:
	sti
	ret

disable_int:
	cli
	ret
