//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _KERNEL_H
#define _KERNEL_H

extern void memfill(unsigned long addr,unsigned long size,unsigned char value);
extern void memcpy(unsigned long srcaddr,unsigned long dstaddr,unsigned long size);
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port,unsigned char byte);
extern void sti(void);
extern void cli(void);

#endif

