;==========================================================
;Project Name:	S.C. Operating System
;Version:	0.02
;Auther:	Shen Chi
;==========================================================
[bits 32]

[section .text]
[extern _do_timer]
[extern _fdc_intr]
[extern _null_fdc_intr]
[extern _no_page]
[global _isr0]
[global _isr1]
[global _isr2]
[global _isr3]
[global _isr4]
[global _isr5]
[global _isr6]
[global _isr7]
[global _isr8]
[global _isr9]
[global _isr10]
[global _isr11]
[global _isr12]
[global _isr13]
[global _isr14]
[global _isr15]
[global _isr16]
[global _isr17]
[global _isr18]
[global _isr19]
[global _irq0]
[global _irq1]
[global _irq6]
[global _switchto]
[global _isr0x40]
[extern _puts]
[extern _puthex]
errmsg	db	"Error!! Exception:",0

_isr0:
	mov eax,0
	jmp $
_isr1:
	mov eax,1
	jmp $
_isr2:
	mov eax,2
	jmp $
_isr3:
	mov eax,3
	jmp $
_isr4:
	mov eax,4
	jmp $
_isr5:
	mov eax,5
	jmp $
_isr6:
	mov eax,6
	jmp $
_isr7:
	mov eax,7
	jmp $
_isr8:
	mov ax,0x10
	mov ds,ax
	mov eax,errmsg
	push eax
	call _puts
	pop eax
	call _puthex
	pop eax
	mov eax,8
	push eax
	call _puthex
	pop eax
	call _puthex
	pop eax
	call _puthex
	jmp $
_isr9:
	mov eax,9
	jmp $
_isr10:
	mov ax,0x10
	mov ds,ax
	mov eax,errmsg
	push eax
	call _puts
	pop eax
	call _puthex
	pop eax
	mov eax,10
	push eax
	call _puthex
	pop eax
	call _puthex
	pop eax
	call _puthex
	jmp $
_isr11:
	mov ax,0x10
	mov ds,ax
	mov eax,errmsg
	push eax
	call _puts
	pop eax
	call _puthex
	pop eax
	mov eax,11
	push eax
	call _puthex
	pop eax
	call _puthex
	pop eax
	call _puthex
	jmp $
_isr12:
	mov ax,0x10
	mov ds,ax
	mov eax,errmsg
	push eax
	call _puts
	pop eax
	call _puthex
	pop eax
	mov eax,12
	push eax
	call _puthex
	pop eax
	call _puthex
	pop eax
	call _puthex
	jmp $
_isr13:
	mov ax,0x10
	mov ds,ax
	mov eax,errmsg
	push eax
	call _puts
	pop eax
	call _puthex
	pop eax
	mov eax,13
	push eax
	call _puthex
	pop eax
	call _puthex
	pop eax
	call _puthex
	jmp $
_isr15:
	mov eax,15
	jmp $
_isr16:
	mov eax,16
	jmp $
_isr17:
	mov ax,0x10
	mov ds,ax
	mov eax,errmsg
	push eax
	call _puts
	pop eax
	call _puthex
	pop eax
	mov eax,17
	push eax
	call _puthex
	pop eax
	call _puthex
	pop eax
	call _puthex
	jmp $
_isr18:
	mov eax,18
	jmp $
_isr19:
	mov eax,19
	jmp $
	iret

_isr14:
	pushad
	push ds
	push es
	push fs
	push gs
	mov ax,0x10
	mov ds,ax

	mov eax,dword [ss:esp+0x30]
	push eax
	mov eax,cr2
	push eax
	call _no_page
	add esp,0x8

	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp,4
	iret
isr14errcode:
	dd 0

send_eoi:
	mov al,0x20
	out 0xa0,al
	out 0x20,al
	ret

_irq0:
	pushad
	push ds
	push es
	push fs
	push gs

	mov ax,0x10
	mov ds,ax

	call send_eoi
	
	mov eax,dword [ss:esp+0x34]
	push eax
	call _do_timer
	pop eax

	pop gs
	pop fs
	pop es
	pop ds
	popad
	iret


_switchto:
	mov eax,dword [ss:esp+4]
	mov word [tss_sel],ax
	db 0xea
	dd 0
tss_sel:
	dw 0xFFFF
	ret

[extern _key_intr]

_irq1:
	pushad
	push ds
	mov eax,0x10
	mov ds,ax

	call _key_intr
	
irq1end:
	call send_eoi
	pop ds
	popad
	iret


_irq6:
	pushad
	push ds
	push es
	push fs
	push gs
	mov ax,0x10
	mov ds,ax

	mov eax,dword [_null_fdc_intr]
	xchg eax,dword [_fdc_intr]
	call eax
	
	call send_eoi
	pop gs
	pop fs
	pop es
	pop ds
	popad
	iret

[extern _sys_call]

_isr0x40:
	push ds
	push es
	push fs
	push gs

	push eax
	mov eax,0x10
	mov ds,ax
	mov es,ax
	pop eax
	
	push edx
	push ecx
	push ebx
	push eax
	call _sys_call
	pop eax
	pop ebx
	pop ecx
	pop edx
	
	pop gs
	pop fs
	pop es
	pop ds
	iret


