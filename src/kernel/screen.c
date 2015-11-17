//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "screen.h"
#include "kernel.h"
#include "task.h"
#include "tm.h"

#define vbuf_addr 0xb8000

unsigned short curx,cury;
unsigned short curpos;
unsigned char charattr=7;

void init_screen(void);
void scroll(void);
void setcursor(unsigned short x,unsigned short y);
void getcursor(unsigned short* x,unsigned short* y);
void setcharattr(unsigned char attr);
void putchar(unsigned char c);
void puts(char* str);
void puthex(unsigned long hex);
void cls(void);
void refresh_vbuf(void);
void putbitchar(unsigned char c,long x,long y);

void refresh_vbuf(void){
	if(cur_vbuf==0)return;
	if(current!=cur_task)return;
	memcpy(cur_vbuf,vbuf_addr,0xFA0);
}

void memfill4scr(unsigned long addr,unsigned long size,unsigned char value,unsigned char cattr){
	asm("1:stosb;xchg %%bl,%%al;stosb;xchg %%bl,%%al;loop 1b"::"c"(size),"D"(addr),"a"(value),"b"(cattr));
}

void scroll(){
	__asm__("mov $0xb8000,%edi;mov %edi,%esi;add $0xA0,%esi;mov $0xF00,%ecx;rep movsb");
	//__asm__("mov $0xb8F00,%edi;mov $0,%al;mov $0xA0,%ecx;rep stosb");
	memfill4scr(0xb8F00, 0xA0/2, ' ', charattr);
}

void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void refreshcursor(void){
	unsigned tmppos=curpos/2;
	outportb(0x3D4, 14);
	outportb(0x3D5, tmppos >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, tmppos);
}

void getcursor(unsigned short* x,unsigned short* y){
	*x=curx;
	*y=cury;
}

void setcursor(unsigned short x,unsigned short y){
	curx=x;
	cury=y;
	curpos=160*(y-1)+2*(x-1);
	refreshcursor();
}

void cr(void){
	setcursor(1,cury);
	return;
}

void bs(void){
	if(cury>1){
		if(curx>1){
			setcursor(curx-1,cury);
		}else{
			setcursor(1,cury-1);
		}
	}
}

void lf(void){
	if(cury<25){
		setcursor(curx,cury+1);
	}else{
		cr();
		scroll();
	}
}

void inccurx(void){
	if(curx<80){
		setcursor(curx+1,cury);
	}else{
		cr();lf();
	}
}

void setcharattr(unsigned char attr){
	charattr=attr;
}

void putchar(unsigned char c){
	if(c==0x08){
		bs();
	}else if(c==0x09){
		inccurx();
	}else if(c=='\r'){
		cr();
	}else if(c=='\n'){
		lf();
	}else if(c>=' '){
		__asm__("mov %0,(%2);inc %2;mov %1,(%2)"::"a"(c),"b"(charattr),"D"(vbuf_addr+curpos));
		inccurx();
	}
}

void puts(char* str){
	while(*str!=0){
		putchar(*str);
		str++;
	}
}

unsigned char tmp;
unsigned char i;
void puthex(unsigned long hex){
	for(i=0;i<8;i++){
		tmp=(hex&0xf0000000)>>28;
		if((tmp<=9)&&(tmp>=0))tmp+=48;
		if((tmp>=10)&&(tmp<=15))tmp+=55;
		putchar(tmp);
		hex<<=4;
	}
}

void cls(void){
	asm("1:stosb;xchg %%bl,%%al;stosb;xchg %%bl,%%al;loop 1b"::"c"(0xFA0/2),"D"(vbuf_addr),"a"(' '),"b"(charattr));
}

void init_screen(void){
	setcursor(1,1);
	cls();
}

void putasc(char c,long x,long y){
	asm("mov %0,(%2);inc %2;mov %1,(%2)"::"a"(c),"b"(charattr),"D"(vbuf_addr+(y-1)*160+(x-1)*2));
}

void putbitchar(unsigned char c,long x,long y){
	unsigned char* pbit;
	unsigned char tbit,cbit=*pbit;
	unsigned long i,j;
	i=c*8;
	i+=0xFFA6E;
	pbit=(char*)i;
	for(j=0;j<8;j++){
		cbit=*(pbit+j);
		for(i=0;i<8;i++){
			tbit=cbit&(1<<(7-i));
			if(tbit!=0){
				putasc(219,x+i,y+j);
			}
		}
	}
}

void putbitstr(char* str,unsigned long x,unsigned long y,unsigned long w){
	while(*str!=0){
		putbitchar(*str,x,y);
		x+=w;
		str++;
	}
}

