//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _SCREEN_H
#define _SCREEN_H

extern void init_screen(void);
extern void scroll(void);
extern void setcursor(unsigned short x,unsigned short y);
extern void getcursor(unsigned short* x,unsigned short* y);
extern void setcharattr(unsigned char attr);
extern void putchar(unsigned char c);
extern void puts(char* str);
extern void puthex(unsigned long hex);
extern void cls(void);
extern void refresh_vbuf(void);
extern void putbitchar(unsigned char c,long x,long y);
extern void putbitstr(char* str,unsigned long x,unsigned long y,unsigned long w);

#endif

