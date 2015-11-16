#include "screen.h"
#include "common.h"

unsigned long i;

unsigned char vbuf[0xFA0*2-1];
unsigned long vbuf_addr;

unsigned char charattr=7;

unsigned long cx,cy;
char *tmpstr;

void init_screen(void);
void setcharattr(unsigned char data);
void refresh(void);
void putchar(unsigned char c,unsigned long x,unsigned long y);
void putch(unsigned char c);
void putstr(char *str);
void getcursor(unsigned short* x,unsigned short* y);
void setcursor(unsigned short x,unsigned short y);
void bs(void);
void lf(void);
void inccurx(void);
void cr(void);
void putchfoprint(unsigned char c);
void scroll(void);
void clear(void);
void puthex(unsigned long hex);
unsigned char getchar(unsigned long x,unsigned long y);
void memfill4scr(unsigned long addr,unsigned long size,unsigned char value,unsigned char cattr);

void init_screen(void){
	for(i=0;i<0xFA0*2;i++)vbuf[i]=0;
	__asm__("mov $0x300,%eax;int $0x40");
	__asm__("mov $0x301,%%eax;int $0x40"::"b"((unsigned long)&vbuf[0]));
	vbuf_addr=0;
	setcursor(1,1);
}

void setcharattr(unsigned char data){
	charattr=data;
}

void memfill4scr(unsigned long addr,unsigned long size,unsigned char value,unsigned char cattr){
	asm("1:stosb;xchg %%bl,%%al;stosb;xchg %%bl,%%al;loop 1b"::"c"(size),"D"(addr),"a"(value),"b"(cattr));
}

void scroll(void){
	if(vbuf_addr<0xFA0){
		vbuf_addr+=0xA0;
		memfill4scr((unsigned long)&vbuf[0]+0xF00+vbuf_addr,0xA0/2,' ',charattr);
	}else{
		vbuf_addr=0;
		memcpy((unsigned long)&vbuf[0]+0x1040,(unsigned long)&vbuf[0],0xF00);
		memfill4scr((unsigned long)&vbuf[0]+0xF00+vbuf_addr,0xA0/2,' ',charattr);
	}
	__asm__("mov $0x301,%%eax;int $0x40"::"b"((unsigned long)&vbuf[0]+vbuf_addr));
}

void clear(void){
	memfill4scr((unsigned long)&vbuf[0]+vbuf_addr,0xFA0/2,' ',charattr);
	cx=1;cy=1;
}

void refresh(void){
	__asm__("mov $0x5,%eax;int $0x40");
}

void putchar(unsigned char c,unsigned long x,unsigned long y){
	unsigned long tmppos=(y-1)*160+(x-1)*2+vbuf_addr;
	vbuf[tmppos]=c;
	vbuf[tmppos+1]=charattr;
}

void getcursor(unsigned short* x,unsigned short* y){
	*x=cx;
	*y=cy;
}

void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void setcursor(unsigned short x,unsigned short y){
	cx=x;
	cy=y;
	unsigned tmppos=(cy-1)*80+(cx-1);
    outportb(0x3D4, 14);
    outportb(0x3D5, tmppos >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, tmppos);
}


void bs(void){
	if(cx>1){
		setcursor(cx-1,cy);
	}else{
		if(cy>1){
			setcursor(80,cy-1);
		}
	}
}

void lf(void){
	if(cy<25){
		setcursor(cx,cy+1);
	}else{
		cr();
		scroll();
	}
}

void inccurx(void){
	if(cx<80){
		setcursor(cx+1,cy);
	}else{
		cr();lf();
	}
}

void cr(void){
	setcursor(1,cy);
}

unsigned char getchar(unsigned long x,unsigned long y){
	unsigned long tmppos=(y-1)*160+(x-1)*2+vbuf_addr;
	return vbuf[tmppos];
}

void putch(unsigned char c){
	if(c==0x08){
		bs();putchar(' ',cx,cy);
	}else if(c==0x09){
		inccurx();
	}else if(c=='\n'){
		cr();lf();
	}else if(c=='\r'){
		cr();
	}else if((c>=' ')&&(c!=27)){
		putchar(c,cx,cy);
		inccurx();
	}
}


void putstr(char *str){
	tmpstr=str;
	while(*tmpstr!=0){
		putch(*tmpstr);
		tmpstr++;
	}
}

void puthex(unsigned long hex){
	unsigned char tmp;
	unsigned char i;
	for(i=0;i<8;i++){
		tmp=(hex&0xf0000000)>>28;
		if((tmp<=9)&&(tmp>=0))tmp+=48;
		if((tmp>=10)&&(tmp<=15))tmp+=55;
		putch(tmp);
		hex<<=4;
	}
}

