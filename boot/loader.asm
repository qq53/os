org	400h

	jmp LABEL_START

%include "fat12hdr.inc"
%include "pm.inc"

LABEL_GDT			Descriptor		0,			0,			0
LABEL_DESC_FLAT_C	Descriptor		0,			0FFFFFh,	DA_CR|DA_32|DA_LIMIT_4K
LABEL_DESC_FLAT_RW	Descriptor		0,			0FFFFFh,	DA_DRW|DA_32|DA_LIMIT_4K
LABEL_DESC_VIDEO	Descriptor		0B8000h,	0FFFFh,		DA_DRW|DA_DPL3

GdtLen		equ	$ - LABEL_GDT
GdtPtr		dw	GdtLen - 1
			dd	BaseOfLoaderPhyAddr + LABEL_GDT

SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT	+ SA_RPL3

BaseOfKernel			equ		8000h
OffsetOfKernel			equ		400h
PageDirBase				equ		100000h
PageTblBase				equ		101000h
BaseOfLoaderPhyAddr		equ		90000h
BaseOfKernelPhyAddr		equ		BaseOfKernel * 10h + OffsetOfKernel
KernelEntryPointPhyAddr	equ		30400h

[SECTION .data]
ALIGN 32
[BITS 32]
_BootMessage		db		'Loading  ',0
_FindFileName		db		'KERNEL  BIN',0
MessageLength		equ		9

_SPValueInRealMode	dw	0
_szPMMessage		db	'[[[Protect Mode]]]',0Ah,0Ah,0
_szMemChkTitle		db	'BaseAddrL BaseAddrH LengthL   LengthH   Type',0Ah,0
_szRamSize			db	'RAM Size :',0
_szReturn			db	0Ah,0
_dwMCRNumber		dd	0
_dwDispPos			dd	(80 * 1 + 0) * 2
_dwMemSize			dd	0
_ARDStruct:
	_dwBaseAddrL	dd	0
	_dwBaseAddrH	dd	0
	_dwLengthLow	dd	0
	_dwLengthHigh	dd	0
	_dwType			dd	0
_MemChkBuf	times 256	db	0

;保护模式
SPValueInRealMode	equ	_SPValueInRealMode + BaseOfLoaderPhyAddr
szPMMessage			equ _szPMMessage + BaseOfLoaderPhyAddr
szMemChkTitle		equ	_szMemChkTitle + BaseOfLoaderPhyAddr
szRamSize			equ _szRamSize + BaseOfLoaderPhyAddr
szReturn			equ _szReturn + BaseOfLoaderPhyAddr
dwMCRNumber			equ _dwMCRNumber + BaseOfLoaderPhyAddr
dwDispPos			equ _dwDispPos + BaseOfLoaderPhyAddr
dwMemSize			equ _dwMemSize + BaseOfLoaderPhyAddr
ARDStruct:			equ _ARDStruct + BaseOfLoaderPhyAddr
	dwBaseAddrL		equ	_dwBaseAddrL + BaseOfLoaderPhyAddr
	dwBaseAddrH		equ _dwBaseAddrH + BaseOfLoaderPhyAddr
	dwLengthLow		equ _dwLengthLow + BaseOfLoaderPhyAddr
	dwLengthHigh	equ _dwLengthHigh + BaseOfLoaderPhyAddr
	dwType			equ _dwType + BaseOfLoaderPhyAddr
MemChkBuf		equ _MemChkBuf + BaseOfLoaderPhyAddr

StackSpace	times	1024	db	0
TopOfStack	equ		BaseOfLoaderPhyAddr + $

[SECTION .16c]
ALIGN 16
[BITS 16]
LABEL_START:
	mov ax,cs
	mov ds,ax
	mov ss,ax
	mov sp,TopOfStack

	xor ah,ah
	xor dl,dl
	int 13h

	;获取内存信息
	mov ebx,0
	mov di,_MemChkBuf
.mem:
	mov eax,0E820h
	mov ecx,20
	mov edx,534D4150h
	int 15h
	jc .fail
	add edi,20
	inc dword [_dwMCRNumber]
	test ebx,ebx
	jnz	.mem
	jmp .memok
.fail:
	mov dword [_dwMCRNumber],0
.memok:

	push BaseOfKernel
	push OffsetOfKernel
	push SecNumOfRootDir
	push SecNumOfRootDir
	push _FindFileName
	call FindFile
	add sp,10
	test ax,ax
	jz LABEL_FIND_KRL
	jmp $
LABEL_FIND_KRL:
	push -1
	push 0
	push OffsetOfKernel
	call ReadFat
	add sp,6

	;加载完Kernel恢复ES
	mov ax,cs
	mov es,ax

	call KillMotor

	;加载GDTR
	lgdt [GdtPtr]
	
	;关中断
	cli

	;打开A20地址线
	mov ax,2401h
	int 15h

	;准备进入保护模式
	mov eax,cr0
	or eax,1
	mov cr0,eax

	jmp dword SelectorFlatC:(BaseOfLoaderPhyAddr+LABEL_PM_START)

KillMotor:
	push dx
	mov dx,3F2h
	mov al,0
	out dx,al
	pop dx
	ret

%include "fat.inc"

[SECTION .s32]
ALIGN 32
[BITS 32]
LABEL_PM_START:
	mov ax,SelectorVideo
	mov gs,ax
	mov ax,SelectorFlatRW
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov ss,ax
	mov esp,TopOfStack

	push szPMMessage
	call DispStr
	add esp,4

	push szMemChkTitle
	call DispStr
	add esp,4
	
	call DispMemSize
	;分页机制
	call SetupPage

	call InitKernel

	jmp SelectorFlatC:KernelEntryPointPhyAddr

%include "lib.inc"

SetupPage:
	;计算数量
	xor edx,edx
	mov eax,[dwMemSize]
	shr eax,22
	mov ecx,eax
	test edx,edx
	jz .no_remainder
	inc ecx
.no_remainder:
	push ecx

	mov ax,SelectorFlatRW
	mov es,ax
	mov edi,PageDirBase
	mov eax,PageTblBase | PG_P | PG_USU | PG_RWW
.1:
	stosd
	add eax,4096
	loop .1

	pop eax
	shl eax,10
	mov ecx,eax
	mov edi,PageTblBase
	mov eax,PG_P | PG_USU | PG_RWW
.2:
	stosd
	add eax,4096
	loop .2

	mov eax,PageDirBase
	mov cr3,eax
	mov eax,cr0
	or eax,80000000h
	mov cr0,eax

	ret

DispMemSize:
	push ecx
	push esi
	push edi
	mov ecx,[dwMCRNumber]
	mov esi,MemChkBuf
.loop:
	mov edx,5
	mov edi,ARDStruct
.1:
	push dword [esi]
	call DispInt
	pop eax
	stosd
	add esi,4
	dec edx
	jnz .1
	call DispReturn
	cmp dword [dwType],1
	jnz .2
	mov eax,[dwBaseAddrL]
	add eax,[dwLengthLow]
	cmp eax,[dwMemSize]
	jb .2
	mov [dwMemSize],eax
.2:
	loop .loop

	call DispReturn
	push szRamSize
	call DispStr
	pop eax

	push dword [dwMemSize]
	call DispInt
	pop eax
	
	mov ah,00001100b
	mov al,'M'
	add dword [dwDispPos],4
	mov edi,[dwDispPos]
	mov [gs:edi],ax
	
	shr eax,20
	inc eax
	mov bx,10
.s1:
	xor dx,dx
	div bx
	mov dh,0Ch
	add dl,'0'
	add edi,-2
	mov [gs:edi],dx
	test ax,ax
	jnz .s1

	pop edi
	pop esi
	pop ecx
	ret

;根据程序头吧代码加载到指定位置
InitKernel:
	movzx ecx,word [BaseOfKernelPhyAddr + 2Ch]
	mov esi,[BaseOfKernelPhyAddr + 1Ch]
	add	esi,BaseOfKernelPhyAddr
.begin:
	mov eax,[esi + 0]
	test eax,eax
	jz .NoHeader
	push dword [esi + 10h]
	mov eax,[esi + 4h]
	add eax,BaseOfKernelPhyAddr
	push eax
	push dword [esi + 8h]
	call MemCpy
	add esp,12
.NoHeader:
	add esi,20h
	dec ecx
	jnz .begin

	ret
