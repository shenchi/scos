;==========================================================
;Project Name:	S.C. Operating System
;Version:	0.02
;Auther:	Shen Chi
;==========================================================

[bits 32]
[global _main]
[extern _kernel_init]

_main:
	mov esi,gdt
	mov edi,0
	mov ecx,24
	rep movsb

	mov al,0
	mov ecx,0x7E8
	rep stosb

	lgdt [gdt_48]

	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	mov esp,0x1F00F00
	
	call _kernel_init
	jmp $

gdt_48:
	dw 0x800
	dd 0

align 8
gdt:
	dw 0,0,0,0
	dw 0xFFFF,0x0000,0x9A00,0x00CF
	dw 0xFFFF,0x0000,0x9200,0x00CF

