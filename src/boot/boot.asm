;==========================================================
;Project Name:	S.C. Operating System
;Version:	0.02
;Auther:	Shen Chi
;==========================================================

bits 16
org 0x7c00

%define fat_seg 0x9000
%define fdt_addr 0x1300
%define kernel_seg 0x1000

entry:
	jmp short main
	nop

;==========================================================
bsOEM		db	"SCOS0.02"
bsSectorSize	dw	0x200
bsClustSize	db	1
bsRessect	dw	1
bsFatCount	db	2
bsFDTSize	dw	224
bsTotalSectors	dw	2880
bsMedia		db	0xF0
bsFatSize	dw	9
bsTrackSectors	dw	0x12
bsHeadCount	dw	2
bsHidenSectors	dd	0
bsHugeSectors	dd	0
bsBootDriver	db	0
bsReserv	db	0
bsBootSign	db	0x29
bsVolID		dd	0
bsVolName	db	"SCOSSYSDISK"
bsFSType	db	"FAT12   "
;==========================================================

main:

	cli
	mov ax,cs
	mov ds,ax
	mov es,ax
	sti

loadfat:
	mov ah,2
	mov al,9
	mov cl,2
	mov ch,0
	mov dx,0
	mov bx,fat_seg
	mov es,bx
	xor bx,bx
	int 0x13

loadfdt:
	mov ah,2
	mov al,0xe
	mov cl,2
	mov ch,0
	mov dh,1
	mov dl,0
	mov bx,fat_seg
	mov es,bx
	mov bx,fdt_addr
	int 0x13

findfile:
	mov si,filename
	mov cx,0
findloop:
	mov ax,0x20
	push cx
	mul cl
	add ax,fdt_addr
	mov di,ax
	mov cx,0xb
	repe cmpsb
	jz found
	pop cx
	inc cx
	cmp cx,245
	jz hang
	mov si,filename
	jmp findloop

found:
	mov di,ax
	mov ax,word [es:di+26]
	mov bx,fat_seg
	mov ds,bx
	mov bx,kernel_seg
	mov es,bx
	xor bx,bx
readloop:
	push bx
	push ax
	call LBA2CHS
	mov dl,0
	mov ah,2
	mov al,1
	int 0x13
	pop ax
	push ax
	xor dx,dx
	mov cl,3
	mul cl
	mov cx,2
	div cx
	xor dx,dx
	mov di,ax
	pop ax
	test ax,1
	jz notodd
	mov ax,word [ds:di]
	mov cl,4
	shr ax,cl
	jmp ifend
notodd:
	mov ax,word [ds:di]
	and ax,0xFFF
ifend:
	mov cx,0xFF8
	cmp cx,ax
	js switchtopm
	pop bx
	add bx,0x200
	jmp readloop

hang:	jmp hang

filename db "KERNEL  BIN"

LBA2CHS:
	push bx
	xor cx,cx
	add ax,31
	mov bl,18
	div bl
	mov cl,ah
	inc cl
	xor ah,ah
	mov bl,2
	div bl
	mov ch,al
	mov dh,ah
	pop bx
	ret

switchtopm:

	mov ax,fat_seg
	mov es,ax
	mov ax,0xe801
	int 0x15
	mov word [es:0],ax
	mov word [es:2],bx

	cli
	
	mov al,0x11
	out 0x20,al
	dw 0xeb,0xeb
	out 0xa0,al
	dw 0xeb,0xeb
	mov al,0x20
	out 0x21,al
	dw 0xeb,0xeb
	mov al,0x28
	out 0xa1,al
	dw 0xeb,0xeb
	mov al,0x04
	out 0x21,al
	dw 0xeb,0xeb
	mov al,0x02
	out 0xa1,al
	dw 0xeb,0xeb
	mov al,0x01
	out 0x21,al
	dw 0xeb,0xeb
	out 0xa1,al
	dw 0xeb,0xeb
	mov al,0xFF
	out 0x21,al
	dw 0xeb,0xeb
	out 0xa1,al
	dw 0xeb,0xeb

	call	empty_8042
	mov	al,0xD1
	out	0x64,al
	call	empty_8042
	mov	al,0xDF
	out	0x60,al
	call	empty_8042

	mov ax,cs
	mov ds,ax

	lgdt [gdt_48]
	lidt [idt_48]
	
	mov eax,cr0
	or eax,1
	mov cr0,eax

	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	mov esp,0x1F00F00

	jmp dword 8:0x10000
	jmp $

empty_8042:
	dw	0x00eb,0x00eb
	in	al,0x64
	test	al,2
	jnz	empty_8042
	ret

idt_48:
	dw 0x800
	dd 0x1000
gdt_48:
	dw 0x800
	dd gdt

align 8
gdt:
	dw 0,0,0,0
	dw 0xFFFF,0x0000,0x9A00,0x00CF
	dw 0xFFFF,0x0000,0x9200,0x00CF
	
times 510-($-$$) db 0
db 0x55,0xAA