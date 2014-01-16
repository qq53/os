org 7c00h

BaseOfLoader	equ	9000h
OffsetOfLoader	equ	400h
TopOfStack		equ	OffsetOfLoader

	jmp	LABEL_START
	nop

%include "fat12hdr.inc"

wRootDirLoop	equ		RootDirSectors
LoaderFileName	db		'LOADER  BIN',0

LABEL_START:
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov sp,TopOfStack
;清屏
	mov ax,600h
	mov bx,700h
	xor cx,cx
	mov dx,184Fh
	int 10h
;关软驱
	xor ah,ah
	xor dl,dl
	int 13h

;找Lorder
	push BaseOfLoader
	push OffsetOfLoader
	push SecNumOfRootDir
	push SecNumOfRootDir
	push LoaderFileName
	call FindFile
	add sp,10
	test ax,ax
	jz LABEL_FIND_LOADER1
	jmp $
LABEL_FIND_LOADER1:
	push -1
	push 0
	push OffsetOfLoader
	call ReadFat
	add sp,6
	jmp BaseOfLoader:OffsetOfLoader

FindFile:
	mov bp,sp
	mov word [bp+4],SecNumOfRootDir
	mov ax,[bp+10]
	mov es,ax
	cld
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmp word [bp+6],0
	jz LABEL_NO_LORDER
	dec word [bp+6]
;读扇区
	mov bx,[bp+8]
	mov ax,[bp+4]
	mov cl,1
	call ReadSector
;一个扇区16个目录项
	mov dx,10h
	mov di,[bp+8]
LABEL_SEARCH_SECTOR:
	test dx,dx
	jz LABEL_NEXT_SECTOR
	mov si,[bp+2]
	mov cx,11
;比较现在和目标文件名
	repz cmpsb
	test cx,cx
	jz LABEL_FIND_LORDER
	and di,0FFE0h
	add di,20h
	dec dx
	jmp LABEL_SEARCH_SECTOR
LABEL_NEXT_SECTOR:
	inc word [bp+4]
	jmp LABEL_SEARCH_IN_ROOT_DIR_BEGIN
LABEL_NO_LORDER:
	mov ax,1
	ret
LABEL_FIND_LORDER:
	mov ax,0
	ret

;pLoader,SecNext,wFat1No
ReadFat:
	push bp
	mov bp,sp
	sub sp,2
;第一个扇区偏移	
	add di,0Fh
	mov ax,word [es:di]
	mov word [bp+6],ax
.ReadFat:
;读SectorNext扇区
	mov bx,[bp+4]
	mov ax,RootDirSectors
	add ax,SecNumOfRootDir
	add ax,[bp+6]
	sub ax,2
	mov cl,1
	call ReadSector
	mov ax,[bp+4]
	add ax,200h
	mov word [bp+4],ax
;是否更新FAT1
	xor dx,dx
    mov ax,[bp+6]
	mov bx,341
	div bx
	mov dx,[bp+8]
	inc ax
	test dx,ax
	jz .NextSec
	mov word [bp+8],ax
	xor bx,bx
	mov cl,1
	call ReadSector
	jmp .NextSec
.NextSec:
;求在存储FAT区中的偏移
	mov ax,[bp+8]
	dec ax
;一个FAT区顶多341个项
	imul ax,341
	mov bx,[bp+6]
	sub bx,ax
	mov ax,bx
	and ax,1
	mov byte [bp-2],al
	mov ax,bx
	shr ax,1
	add bx,ax
	inc bx
	xor cx,cx
	
;根据奇偶对应不同读到方式
	mov al,[bp-2]
	test al,al
	jz .00
	jmp .01
.00:
	mov al,byte [es:bx]
	and al,0Fh
	mov cl,byte [es:bx-1]
	jmp .02
.01:
	mov al,byte [es:bx]
	mov cl,byte [es:bx-1]
	and cl,0F0h
.02:
	movzx ax,al
	movzx cx,cl
	mov bl,[bp-2]
	test bl,bl
	jz .03
	jmp .04
.03:
	shl ax,8
	jmp .05
.04:
	shl ax,4
	shr cx,4
.05:
	add ax,cx
	cmp ax,0FFFh
	jz .LoadEnd
	mov word [bp+6],ax
	jmp .ReadFat
.LoadEnd:
	mov sp,bp
	pop bp
	ret

ReadSector:
	push bp
	mov bp,sp

	sub sp,1
	mov byte [bp-1],cl
	push bx
	mov bl,[BPB_SecPerTrk]
	div bl
	inc ah
	mov cl,ah
	mov dh,al
	shr al,1
	mov ch,al
	and dh,1
	pop bx
	mov dl,[BS_DrvNum]
.1:
	mov ah,2
	mov al,byte [bp-1]
	int 13h
	jc .1

	mov sp,bp
	pop bp
	ret

times 510-($-$$)	db	0
dw	0AA55h
