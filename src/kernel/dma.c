//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "dma.h"
#include "kernel.h"

unsigned short preg[7]={0x87,0x83,0x81,0x81,0x8f,0x8b,0x89,0x8a};

void set_8bit_dma(unsigned short channel,unsigned long address,unsigned long count,unsigned char rw){
	unsigned short addrport=channel*2;
	unsigned short countport=(channel*2)+1;
	unsigned short pageport=preg[channel];
	unsigned char tmpcmd=0x40|(rw&3)<<2|(channel&3);
	cli();
	outb(0xA,4|(channel&3));

	outb(0xC,tmpcmd);
	outb(0xB,tmpcmd);

	outb(addrport,address&0xFF);
	outb(addrport,(address>>8)&0xFF);

	outb(pageport,(address>>16)&0xFF);

	outb(countport,count&0xFF);
	outb(countport,(count>>8)&0xFF);
	
	outb(0xA,channel&3);
	sti();
}

