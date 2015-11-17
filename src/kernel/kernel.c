//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "idt.h"
#include "gdt.h"
#include "screen.h"
#include "mem.h"
#include "fdc.h"
#include "dma.h"
#include "fs.h"
#include "task.h"
#include "tm.h"

void memfill(unsigned long addr,unsigned long size,unsigned char value);
void memcpy(unsigned long srcaddr,unsigned long dstaddr,unsigned long size);
unsigned char inb(unsigned short port);
void outb(unsigned short port,unsigned char byte);
void sti(void);
void cli(void);
void sys_call(unsigned long param1,unsigned long param2,unsigned long param3,unsigned long param4);
void key_intr(void);
void show_welcome(void);
void load_shell(void);

int kernel_init(void){
	init_interrupt();
	init_screen();
	show_welcome();
	init_mm();
	init_task();
	init_tm();
	init_timer(100);
	sti();
	init_fdc();
	init_fs();

	setup_sys_call();
	load_shell();
	
	sti();
	__asm__("ltr %%ax"::"a"(3<<3));
	__asm__("ljmp $0x28,$0");
	
	for(;;);
	
	return 0;
}

void show_welcome(void){
	setcharattr(0x1f);
	cls();
	putbitstr("SCOS",12,11,12);
	setcursor(40, 23);
	puts("Author: Shen Chi      Ver 0.02");
	setcursor(1,25);
	puts("Loading...\n\r");
}

void load_shell(void){
	allocate_for_task(1);
	puts("Loading Shell...");
	create_task("SHELL   BIN", 1);
	puts("OK\n\r");
}

void sti(void){
	__asm__("sti");
}

void cli(void){
	__asm__("cli");
}

void memfill(unsigned long addr,unsigned long size,unsigned char value){
	__asm__("1:stosb;loop 1b"::"c"(size),"D"(addr),"a"(value));
	return;
}

void memcpy(unsigned long srcaddr,unsigned long dstaddr,unsigned long size){
	__asm__("1:movsb;loop 1b"::"c"(size),"S"(srcaddr),"D"(dstaddr));
	return;
}

unsigned char inb(unsigned short port){
	unsigned char tmp;
	__asm__("inb %1,%0":"=a"(tmp):"d"(port));
	return tmp;
}
void outb(unsigned short port,unsigned char byte){
	__asm__("outb %0,%1"::"a"(byte),"d"(port));
	__asm__("jmp 1f\n\t1:jmp 1f\n\t1:");
	return;
}

unsigned short khead,ktail;
unsigned char *kbuf;
unsigned char shiftstate=0;

void key_intr(void){
	unsigned char code;
	code=inb(0x60);
	if(code&0x80){
		code&=0x7F;
		if((code==0x2a)|(code==0x36))shiftstate=0;
	}else{
		if((code==0x2a)|(code==0x36)){
			shiftstate=0x80;
		}else{
			code|=shiftstate;
			if(code==0x8f){
				set_cur_task(1);
				return;
			}
			if(cur_kbuf==0)return;
			kbuf=(unsigned char*)cur_kbuf;
			khead=kbuf[0];
			ktail=kbuf[1];
			if(ktail==khead)return;
			kbuf[ktail+2]=code;
			ktail=(ktail+1)%256;
			kbuf[1]=ktail&0xFF;
		}
	}
	return;
}

void sys_call(unsigned long param1,unsigned long param2,unsigned long param3,unsigned long param4){
	unsigned long tah=(param1>>8)&0xFF;
	unsigned long tal=param1&0xFF;
	if(tah==0){										// Screen
		if(tal==0){
			cls();
		}else if(tal==1){
			putchar((param2&0xFF));
		}else if(tal==2){
			setcharattr((param2&0xFF));
		}else if(tal==3){
			getcursor(&param3,&param4);
		}else if(tal==4){
			setcursor(param3,param4);
		}else if(tal==5){
			refresh_vbuf();
		}
	}else if(tah==2){								// Tasks
		if(tal==0){
			create_task_for_ring3(param2);
		}else if(tal==1){
			param1=current;
		}else if(tal==2){
			exit_task_for_ring(param2);
		}else if(tal==3){
			task_list_for_ring3(param2);
		}
	}else if(tah==3){
		//puts("Task Management Events\n\r");
		if(tal==0){
			set_cur_task_without_task_no();
		}else if(tal==1){
			set_cur_vbuf(param2);
		}else if(tal==2){
			set_cur_kbuf(param2);
		}else if(tal==3){
			set_cur_task(param2);
			cls();setcursor(1,1);
			puts("This Task has no vedio buffer!\n\rPlease press [shift]+[tab] to switch to shell!\n\r");
		}
	}else if(tah==4){
		if(tal==0){
			param1=read_file_for_ring3(param2, param3);
		}else if(tal==1){
			param1=change_dir_for_ring3(param2);
		}else if(tal==2){
			param1=create_file_for_ring3(param2, param3, param4);
		}else if(tal==3){
			dir_for_ring3(param2);
		}else if(tal==4){
			param1=delete_file_for_ring3(param2);
		}else if(tal==5){
			param1=create_dir_for_ring3(param2, param3, param4);
		}
	}
}

